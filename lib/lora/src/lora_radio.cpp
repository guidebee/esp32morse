//
// Created by Jing SHEN on 19/9/20.
//

#include "lora_radio.hpp"

#include "configuration.hpp"
#include <string>

#define SEND_HELLO_PERIOD 15000

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
            printf("%s", loRaData.c_str());
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
    std::string encodedMessage = encodeMessage(type, message);
    print(encodedMessage.c_str());
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
            std::string::size_type sz;
            int c = std::atoi(counter.c_str());
            int type = std::atoi(messageType.c_str());
            int len = std::atoi(messageLength.c_str());

            _loraMessage.payload = payload;
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