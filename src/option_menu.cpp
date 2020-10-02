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
    drawClientArea();
}

void OptionMenu::onRightReleased() {
    currentSelect += 1;
    if (currentSelect > upperMenuIndex) currentSelect = upperMenuIndex;
    drawClientArea();
}


void OptionMenu::onOkReleased() {
    switch (currentSelect) {
        case 0:
            menuOption = 0;

            break;
        case 1:
            menuOption = 1;
            topBar.displayText("    Play Sound", topBarPattern);
            currentSelect = 0;
            drawClientArea();

            break;
        case 2:
            menuOption = 2;
            topBar.displayText("    Bluetooth", topBarPattern);
            currentSelect = 1;
            drawClientArea();
            break;
        case 3:
            menuOption = 3;
            break;
        case 4:
            menuOption = 4;
            break;
        case 5:
            topBar.displayText("   Input Speed", topBarPattern);
            currentSelect = 1;

            menuOption = 5;
            drawClientArea();

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

void OptionMenu::drawOptionMenus(int extraOffsetY) {
    for (int i = 0; i <= upperMenuIndex; i++) {
        int x = 0;
        int y = (i + 1) * CHAR_HEIGHT + extraOffsetY;
        if (currentSelect == i) {
            display.fillRect(x, y, SCREEN_WIDTH, CHAR_HEIGHT, WHITE);
            display.setTextColor(BLACK);
        } else {
            display.setTextColor(WHITE);
        }
        display.setCursor(x + OFFSET_X, y);
        display.print((options[i] + "\n").c_str());
    }
}

void OptionMenu::drawClientArea() {
    clearClient();
    int extraOffsetY = 0;
    switch (menuOption) {
        case 1:
        case 2:
            options[0] = "On";
            options[1] = "Off";
            extraOffsetY = CHAR_HEIGHT;
            upperMenuIndex = 1;
            drawOptionMenus(extraOffsetY);
            break;
        case 5:
            options[0] = "Fast";
            options[1] = "Normal";
            extraOffsetY = CHAR_HEIGHT;
            upperMenuIndex = 1;
            drawOptionMenus(extraOffsetY);
            break;
        default:
            options[0] = "Device Name";
            options[1] = "Play Sound";
            options[2] = "Bluetooth";
            options[3] = "Channel Id";
            options[4] = "Sync Word";
            options[5] = "Input Speed";
            options[6] = "Exit";
            upperMenuIndex = 6;
            drawOptionMenus(extraOffsetY);
            break;


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
    drawClientArea();
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
    drawClientArea();
}
