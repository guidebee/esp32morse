//
// Created by Jing SHEN on 1/10/20.
//

#include "option_menu.hpp"

extern bool isOptionMode;


void OptionMenu::onMainReleased() {

}


void OptionMenu::onLeftReleased() {
    currentSelect -= 1;
    if (currentSelect < 0) currentSelect = 0;
    drawOptionMenus();
}

void OptionMenu::onRightReleased() {
    currentSelect += 1;
    if (currentSelect > 6) currentSelect = 6;
    drawOptionMenus();
}


void OptionMenu::onOkReleased() {
    switch (currentSelect) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            isOptionMode = false;
            break;
    }
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

void OptionMenu::drawOptionMenus() {
    clearClient();
    for (int i = 0; i < options->size(); i++) {
        int x = 0;
        int y = (i + 1) * CHAR_HEIGHT;
        if (currentSelect == i) {

            display.fillRect(x, y, SCREEN_WIDTH, CHAR_HEIGHT, WHITE);
            display.setTextColor(BLACK);
        } else {
            display.setTextColor(WHITE);
        }
        display.setCursor(x, y);
        display.print((options[i] + "\n").c_str());
    }
    display.display();

}
