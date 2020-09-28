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

            decodeMessage(loRaData.c_str());
            loraMessageListener->onMessageReceived(_loraMessage);
        }
    }
    auto current_mills = millis();
    int diff = static_cast<int>(current_mills - _last_time);
    if (diff > SEND_HELLO_PERIOD) {
        _last_time = current_mills;
        sendHello();
    }
}


void LoraRadioClass::sendMessage(std::string message, int type) {
    beginPacket();
    std::string encrypted = encryptPayload(message);
    std::string encodedMessage = encodeMessage(type, encrypted);
    print(encodedMessage.c_str());
////    std::string encrypted=encryptPayload(message);
//    std::string decrypted = decryptPayload(encrypted);
//    Serial.printf("encrypted:%s\n", encrypted.c_str());
//    Serial.printf("decrypted:%s\n", decrypted.c_str());
    endPacket();

}

void LoraRadioClass::sendHello() {
    sendMessage(globalConfiguration.deviceName, MESSAGE_TYPE_HELLO);

}

void LoraRadioClass::addListener(LoraMessageListener *listener) {
    loraMessageListener = listener;
}

std::string LoraRadioClass::encodeMessage(int type, std::string message) {
    char buffer[128];
    int len = message.length();
    sprintf(buffer, "%6s%12s%12s%06d%02d%02d%8s%s",
            globalConfiguration.channelId.c_str(),
            globalConfiguration.chipId.c_str(),
            "000000000000",
            _counter,
            type,
            len,
            "00000000",
            message.c_str());
    if (type != MESSAGE_TYPE_HELLO) {
        _counter += 1;
    }
    return buffer;

}

void LoraRadioClass::decodeMessage(std::string message) {

    int len = message.length();
    if (len > 48) {
        if (message[0] == '<' && message[5] == '>') {
            std::string channelId = message.substr(0, 6);
            std::string chipId = message.substr(6, 12);
            std::string repeaterId = message.substr(18, 12);
            std::string counter = message.substr(30, 6);
            std::string messageType = message.substr(36, 2);
            std::string messageLength = message.substr(38, 2);
            std::string reserved = message.substr(40, 8);
            std::string payload = message.substr(48, len - 48);
            int c = std::atoi(counter.c_str());
            int type = std::atoi(messageType.c_str());
            int len = std::atoi(messageLength.c_str());

            std::string decrypted = payload;
            try{
                decrypted=decryptPayload(payload);
            } catch (std::exception ) {

            }
            _loraMessage.payload = decrypted;
            _loraMessage.messageType = type;
            _loraMessage.length = len;
            _loraMessage.channelId = channelId;
            _loraMessage.chipId = chipId;
            _loraMessage.repeaterId = repeaterId;
            _loraMessage.counter = c;
            _loraMessage.reserved = reserved;

        }
    }


}


std::string LoraRadioClass::encryptPayload(std::string payload) {
    unsigned char cipherTextOutput[128];
    for (unsigned char & i : cipherTextOutput) i = 0;
    ::encrypt(const_cast<char *>(payload.c_str()), globalConfiguration.encryptionKey, cipherTextOutput);
    size_t outputLength;
    unsigned char *encoded = base64_encode((const unsigned char *) cipherTextOutput, strlen(
            reinterpret_cast<const char *>(cipherTextOutput)), &outputLength);
    sprintf(reinterpret_cast<char *>(cipherTextOutput), "%.*s", outputLength, encoded);
    free(encoded);
    return reinterpret_cast<const char *>(cipherTextOutput);
}

std::string LoraRadioClass::decryptPayload(std::string payload) {
    size_t outputLength;
    unsigned char cipherTextOutput[128];
    for (unsigned char & i : cipherTextOutput) i = 0;
    auto encrypted = reinterpret_cast<const unsigned char *>(payload.c_str());
    unsigned char *decoded = base64_decode((const unsigned char *) encrypted, strlen(
            reinterpret_cast<const char *>(encrypted)), &outputLength);
    ::decrypt(decoded, globalConfiguration.encryptionKey, cipherTextOutput);
    free(decoded);
    return reinterpret_cast<const char *>(cipherTextOutput);
}
