//
// Created by Jing SHEN on 27/9/20.
//

#include "keypad.hpp"

Keypad::Keypad() : _mainButton(MAIN_BUTTON_PIN),
                   _leftButton(LEFT_BUTTON_PIN),
                   _okButton(OK_BUTTON_PIN), _rightButton(RIGHT_BUTTON_PIN) {

}

void Keypad::setup() {
    _mainButton.setup();
    _leftButton.setup();
    _rightButton.setup();
    _okButton.setup();
}

void Keypad::loop() {
    _mainButton.loop();
    _leftButton.loop();
    _rightButton.loop();
    _okButton.loop();

    if(keypadListener!= nullptr){
        if(_mainButton.isPressed()){
            keypadListener->onMainPressed();
        }
        if(_mainButton.isReleased()){
            keypadListener->onMainReleased();
        }
        if(_leftButton.isPressed()){
            keypadListener->onLeftPressed();
        }
        if(_leftButton.isReleased()){
            keypadListener->onLeftReleased();
        }
        if(_rightButton.isPressed()){
            keypadListener->onRightPressed();
        }
        if(_rightButton.isReleased()){
            keypadListener->onRightReleased();
        }
        if(_okButton.isPressed()){
            keypadListener->onOkPressed();
        }
        if(_okButton.isReleased()){
            keypadListener->onOkReleased();
        }
    }

}

void Keypad::addListener(KeypadListener *listener) {
    keypadListener = listener;
}
