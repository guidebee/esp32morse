//
// Created by Jing SHEN on 19/9/20.
//

#ifndef ESP32MORSE_LORA_RADIO_HPP
#define ESP32MORSE_LORA_RADIO_HPP

#include <LoRa.h>

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
#define BAND 915E6

class LoraRadioClass :public LoRaClass{
public:
    void init() ;
};


#endif //ESP32MORSE_LORA_RADIO_HPP
