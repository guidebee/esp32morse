//
// Created by Jing SHEN on 19/9/20.
//

#include "lora_radio.hpp"
#include "mbedtls/aes.h"
#include "configuration.hpp"
#include <string>

extern "C" {
#include "crypto/base64.h"
}

#define SEND_HELLO_PERIOD 15000
#define BUFFER_SIZE 200

void encrypt(char *plainText, char *key, unsigned char *outputBuffer) {
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, (const unsigned char *) key, strlen(key) * 8);
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char *) plainText, outputBuffer);
    mbedtls_aes_free(&aes);
}

void decrypt(unsigned char *cipherText, char *key, unsigned char *outputBuffer) {
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, (const unsigned char *) key, strlen(key) * 8);
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char *) cipherText, outputBuffer);
    mbedtls_aes_free(&aes);
}

void LoraRadioClass::setup() {
    //SPI LoRa pins
    SPI.begin(SCK, MISO, MOSI, SS);
    //setup LoRa transceiver module

    setPins(SS, RST, DIO0);

    if (!begin(BAND)) {
        Serial.println("Starting LoRa failed!");
        while (true);
    }

    setSyncWord(_syncWord);
    sendHello();
}

void LoraRadioClass::loop() {
    int packetSize = parsePacket();
    String loRaData;
    if (packetSize) {
        //read packet
        while (available()) {
            loRaData += readString();
        }
        if (loraMessageListener != nullptr) {
            int rssi = packetRssi();
            _loraMessage.rssi = rssi;
            decodeMessage(loRaData.c_str());
            if (_loraMessage.valid) {
                loraMessageListener->onMessageReceived(_loraMessage);
                if (_loraMessage.messageType == MESSAGE_TYPE_TEXT) {
                    sendAck();
                }
            }
        }
    }
    auto current_mills = millis();
    int diff = static_cast<int>(current_mills - _last_time);
    if (diff > SEND_HELLO_PERIOD) {
        _last_time = current_mills;
        sendHello();
    }
}


void LoraRadioClass::sendMessage(std::string message, int type, bool encrypted) {
    beginPacket();

    std::string encryptedMessage;
    if (encrypted) {
        encryptedMessage = encryptPayload(message);
    } else {
        encryptedMessage = message;
    }
    bool encryptedSuccessfully=!encryptedMessage.compare(message)==0;
    std::string encodedMessage = encodeMessage(type, encryptedMessage, encryptedSuccessfully);
    print(encodedMessage.c_str());
    endPacket();

}

void LoraRadioClass::sendHello() {
    if (globalConfiguration.deviceName.empty()) {
        globalConfiguration.deviceName = DEFAULT_DEVICE_NAME;
    }
    sendMessage(globalConfiguration.deviceName, MESSAGE_TYPE_HELLO, false);

}

void LoraRadioClass::sendAck() {
    if (globalConfiguration.deviceName.empty()) {
        globalConfiguration.deviceName = DEFAULT_DEVICE_NAME;
    }
    sendMessage(globalConfiguration.deviceName, MESSAGE_TYPE_TEXT_ACK, false);
}


void LoraRadioClass::addListener(LoraMessageListener *listener) {
    loraMessageListener = listener;
}

std::string LoraRadioClass::encodeMessage(int type, std::string message, bool encrypted) {
    char buffer[BUFFER_SIZE];
    int len = message.length();
    sprintf(buffer, "%6s%12s%12s%05d%01d%02d%02d%8s%s",
            globalConfiguration.channelId.c_str(),
            globalConfiguration.chipId.c_str(),
            "000000000000",
            _counter,
            encrypted,
            type,
            len,
            "00000000",
            message.c_str());
    if (type != MESSAGE_TYPE_HELLO) {
        _counter += 1;
    }
    return buffer;

}

void LoraRadioClass::decodeMessage(String rawMessage) {

    int len = rawMessage.length();
    _loraMessage.valid = false;
    if (len > 48) {
        std::string message = rawMessage.c_str();
        if (message[0] == '<' && message[5] == '>') {
            std::string channelId = message.substr(0, 6);
            std::string messageLength = message.substr(38, 2);
            std::string payload = message.substr(48, len - 48);
            int len = std::atoi(messageLength.c_str());
            if (channelId.compare(globalConfiguration.channelId) == 0 && len == payload.length()) {
                std::string chipId = message.substr(6, 12);
                std::string repeaterId = message.substr(18, 12);
                std::string counter = message.substr(30, 5);
                std::string encryptedFlag = message.substr(35, 1);
                std::string messageType = message.substr(36, 2);
                std::string reserved = message.substr(40, 8);

                int c = std::atoi(counter.c_str());
                int type = std::atoi(messageType.c_str());
                bool encrypted = std::atoi(encryptedFlag.c_str());
                std::string decrypted = payload;
                try {
                    if (encrypted) {
                        decrypted = decryptPayload(payload);
                    }
                } catch (std::exception) {

                }
                _loraMessage.payload = decrypted;
                _loraMessage.messageType = type;
                _loraMessage.length = len;
                _loraMessage.channelId = channelId;
                _loraMessage.chipId = chipId;
                _loraMessage.repeaterId = repeaterId;
                _loraMessage.counter = c;
                _loraMessage.reserved = reserved;
                _loraMessage.valid = true;
            }
        }
    }
}

std::string LoraRadioClass::encryptPayload(std::string payload) {
    unsigned char cipherTextOutput[BUFFER_SIZE];
    for (unsigned char &i : cipherTextOutput) i = 0;
    ::encrypt(const_cast<char *>(payload.c_str()), globalConfiguration.encryptionKey, cipherTextOutput);
    size_t outputLength;
    unsigned char *encoded = base64_encode((const unsigned char *) cipherTextOutput, strlen(
            reinterpret_cast<const char *>(cipherTextOutput)), &outputLength);
    sprintf(reinterpret_cast<char *>(cipherTextOutput), "%.*s", outputLength, encoded);
    free(encoded);
    std::string output = reinterpret_cast<const char *>(cipherTextOutput);
    std::string decrypted = decryptPayload(output);
    if (decrypted.compare(payload) == 0) {
        return output;
    } else {
        return payload;
    }
}

std::string LoraRadioClass::decryptPayload(std::string rawPayload) {
    try {
        std::string payload = rawPayload.erase(rawPayload.find_last_not_of(" \n\r\t") + 1);
        size_t outputLength;
        unsigned char cipherTextOutput[BUFFER_SIZE];
        for (unsigned char &i : cipherTextOutput) i = 0;
        auto encrypted = reinterpret_cast<const unsigned char *>(payload.c_str());
        unsigned char *decoded = base64_decode((const unsigned char *) encrypted, strlen(
                reinterpret_cast<const char *>(encrypted)), &outputLength);
        ::decrypt(decoded, globalConfiguration.encryptionKey, cipherTextOutput);
        free(decoded);
        return reinterpret_cast<const char *>(cipherTextOutput);
    } catch (std::exception) {
        return rawPayload;
    }
}

