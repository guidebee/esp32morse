//
// Created by Jing SHEN on 18/9/20.
//

#ifndef ESP32MORSE_BUZZER_TONE_HPP
#define ESP32MORSE_BUZZER_TONE_HPP

#include <Arduino.h>

int freq = 2000;
int channel = 0;
int resolution = 8;

class BuzzerTone {
public:
    explicit BuzzerTone() {

    }

    void init() {
        ledcSetup(channel, freq, resolution);
        ledcAttachPin(33, channel);
    }
};

#endif //ESP32MORSE_BUZZER_TONE_HPP
