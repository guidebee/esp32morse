//
// Created by Jing SHEN on 27/9/20.
//

#ifndef ESP32MORSE_KEYPAD_HPP
#define ESP32MORSE_KEYPAD_HPP

#define MAIN_BUTTON_PIN 36
#define LEFT_BUTTON_PIN 34
#define RIGHT_BUTTON_PIN 35
#define OK_BUTTON_PIN 23

#include "push_button.hpp"


class KeypadListener {
public:
    virtual void onMainPressed() = 0;
    virtual void onMainReleased() = 0;

    virtual void onLeftPressed() = 0;
    virtual void onLeftReleased() = 0;

    virtual void onRightPressed() = 0;
    virtual void onRightReleased() = 0;

    virtual void onOkPressed() = 0;
    virtual void onOkReleased() = 0;

};


class Keypad {
private:
    PushButton _mainButton;
    PushButton _leftButton;
    PushButton _okButton;
    PushButton _rightButton;

    KeypadListener *keypadListener = nullptr;


public:

    void addListener(KeypadListener *listener);

    explicit Keypad();

    void setup();

    void loop();



};

#endif //ESP32MORSE_KEYPAD_HPP
