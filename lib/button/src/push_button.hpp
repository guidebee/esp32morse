//
// Created by Jing SHEN on 19/9/20.
//

#ifndef ESP32MORSE_PUSH_BUTTON_HPP
#define ESP32MORSE_PUSH_BUTTON_HPP

#include <ezButton.h>

class PushButton : public ezButton {
public:
    explicit PushButton(int pin) : ezButton(pin) {}

    void setup();
};

#endif //ESP32MORSE_PUSH_BUTTON_HPP
