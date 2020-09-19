//
// Created by Jing SHEN on 19/9/20.
//

#include "lora_radio.hpp"

void LoraRadioClass::init() {


    //SPI LoRa pins
    SPI.begin(SCK, MISO, MOSI, SS);
    //setup LoRa transceiver module

    setPins(SS, RST, DIO0);

    if (!begin(BAND)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    setSyncWord(0x34);
}