//
// Created by Jing SHEN on 19/9/20.
//

#include "lora_radio.hpp"

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
        if(loraMessageListener!= nullptr){
            _loraMessage.payload=loRaData.c_str();
            loraMessageListener->onMessageReceived(_loraMessage);
        }
    }
}


void LoraRadioClass::sendMessage(std::string message) {
    beginPacket();
    print(message.c_str());
    endPacket();

}

void LoraRadioClass::addListener(LoraMessageListener *listener) {
    loraMessageListener = listener;
}
