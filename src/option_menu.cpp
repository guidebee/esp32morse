//
// Created by Jing SHEN on 1/10/20.
//

#include "option_menu.hpp"

extern bool isOptionMode;

void OptionMenu::onMainPressed() {

}

void OptionMenu::onMainReleased() {

}

void OptionMenu::onLeftPressed() {

}

void OptionMenu::onLeftReleased() {

}

void OptionMenu::onRightPressed() {

}

void OptionMenu::onRightReleased() {

}

void OptionMenu::onOkPressed() {

}

void OptionMenu::onOkReleased() {

}

void OptionMenu::setup() {
    display.setup();
    receiverLed.setup();
    blueToothLed.setup();
    keypad.setup();
    topBar.clearScreen();
    last_mills = millis();
    current_mills = last_mills;
    last_task_mills = last_mills;
    diff_mills = 0;
    keypad.addListener(this);
    topBar.displayText("     Main Menu", topBarPattern);
    blueToothLed.signalMorseText("Menu");
}

void OptionMenu::loop() {

    topBar.loop();
    receiverLed.loop();
    blueToothLed.loop();
    keypad.loop();
    current_mills = millis();
    unsigned long diff_task_mills = current_mills - last_task_mills;
    if (diff_task_mills > 5000) {
        blueToothLed.signalError();
        receiverLed.signalError();
        Serial.printf("diff_task_mills\n");
        last_task_mills = current_mills;
    }


}
