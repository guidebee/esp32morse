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
            menuOption = 0;

            break;
        case 1:
            menuOption = 1;
//            playSoundToggleView.drawOptionMenus();
            break;
        case 2:
            menuOption = 2;
            // bluetoothToggleView.drawOptionMenus();
            break;
        case 3:
            menuOption = 3;
            break;
        case 4:
            menuOption = 4;
            break;
        case 5:
            menuOption = 5;
            // inputSpeedToggleView.drawOptionMenus();
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
//    playSoundToggleView.setup();
//    bluetoothToggleView.setup();
//    inputSpeedToggleView.setup();
//    playSoundToggleView.addToggleListener(this);
//    bluetoothToggleView.addToggleListener(this);
//    inputSpeedToggleView.addToggleListener(this);


}

void OptionMenu::loop() {

    BaseView::loop();
    receiverLed.loop();
    blueToothLed.loop();

    current_mills = millis();
    unsigned long diff_task_mills = current_mills - last_task_mills;

    switch (menuOption) {


        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            if (diff_task_mills > 5000) {
                blueToothLed.signalError();
                receiverLed.signalError();
                last_task_mills = current_mills;
            }
            break;


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

void OptionMenu::onChooseOn() {
    switch (menuOption) {
        case 1:
            printf("Play sound On");
            break;
        case 2:
            printf("bluetooth On");
            break;
        case 5:
            printf("fast speed");
            break;
        default:
            break;

    }
    menuOption = 1;
    drawOptionMenus();
}

void OptionMenu::onChooseOff() {
    switch (menuOption) {
        case 1:
            printf("Play sound Off");
            break;
        case 2:
            printf("bluetooth Off");
            break;
        case 5:
            printf("normal speed");
            break;
        default:
            break;

    }
    menuOption = -1;
    drawOptionMenus();
}
