//
// Created by Jing SHEN on 1/10/20.
//

#include "option_menu.hpp"

extern bool isOptionMode;


void OptionMenu::onMainReleased() {

}


void OptionMenu::onLeftReleased() {

}

void OptionMenu::onRightReleased() {

}


void OptionMenu::onOkReleased() {

}

void OptionMenu::setup() {
    BaseView::setup();
    receiverLed.setup();
    blueToothLed.setup();
    blueToothLed.signalMorseText("Menu");
}

void OptionMenu::loop() {

    BaseView::loop();
    receiverLed.loop();
    blueToothLed.loop();

    current_mills = millis();
    unsigned long diff_task_mills = current_mills - last_task_mills;
    if (diff_task_mills > 5000) {
        blueToothLed.signalError();
        receiverLed.signalError();
        last_task_mills = current_mills;
    }


}
