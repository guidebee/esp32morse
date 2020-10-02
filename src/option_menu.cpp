//
// Created by Jing SHEN on 1/10/20.
//

#include "option_menu.hpp"
#include "configuration.hpp"

extern bool isOptionMode;
extern Configuration globalConfiguration;

void OptionMenu::onMainReleased() {

}


void OptionMenu::onLeftReleased() {
    currentSelect -= 1;
    if (currentSelect < 0) currentSelect = upperMenuIndex;
    drawClientArea();
}

void OptionMenu::onRightReleased() {
    currentSelect += 1;
    if (currentSelect > upperMenuIndex) currentSelect = 0;
    drawClientArea();
}


void OptionMenu::onOkReleased() {
    switch (currentSelect) {
        case 0:
            topBar.displayText("   Device Name", topBarPattern);
            menuOption = 0;
            drawClientArea();
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
            topBar.displayText("     Channel Id", topBarPattern);
            drawClientArea();
            break;
        case 4:
            menuOption = 4;
            topBar.displayText("     Sync Word", topBarPattern);
            drawClientArea();
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
    char buffer[64];
    switch (menuOption) {
        case 0:
            drawInputValue(globalConfiguration.deviceName);
            drawFullKeyboard();
            drawControlKeyboard();
            break;
        case 1:
        case 2:
            options[0] = "On";
            options[1] = "Off";
            extraOffsetY = CHAR_HEIGHT;
            upperMenuIndex = 1;
            drawOptionMenus(extraOffsetY);
            break;
        case 3:
            sprintf(buffer, "<%04d>", globalConfiguration.channelIdValue);
            drawInputValue(buffer);
            drawNumberKeyboard();
            drawControlKeyboard();
            break;
        case 4: {

            sprintf(buffer, "%d", globalConfiguration.syncWord);
            drawInputValue(buffer);
            drawNumberKeyboard();
            drawControlKeyboard();
        }
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

void OptionMenu::drawFullKeyboard() {
    int x = OFFSET_X;
    int y = CHAR_HEIGHT * 3;
    for (int i = 0; i < 36; i++) {
        int col = i % 12;
        int row = static_cast<int>( i / 12 );
        if (letterSelected == i) {
            display.fillRect(x + col * 10 - 1, y + row * 10 - 1, 10, 10, WHITE);
            display.setTextColor(BLACK);
        } else {
            display.setTextColor(WHITE);
        }
        display.setCursor(x + col * 10 + 1, y + row * CHAR_HEIGHT);
        display.print(fullKeyboard[i]);
    }

}

void OptionMenu::drawNumberKeyboard() {
    int x = OFFSET_X;
    int y = CHAR_HEIGHT * 5;

    for (int i = 0; i < 12; i++) {
        display.setCursor(x + i * 10, y);
        if (numberSelected == i) {
            display.fillRect(x + i * 10 - 2, y - 1, 10, 10, WHITE);
            display.setTextColor(BLACK);
        } else {
            display.setTextColor(WHITE);
        }
        display.print(numberKeyboard[i]);
    }
}

void OptionMenu::drawControlKeyboard() {
    int x = OFFSET_X;
    int y = CHAR_HEIGHT * 3 + 30;
    for (int i = 0; i < 3; i++) {
        display.setCursor(x + i * 20, y);
        if (controlSelected == i) {
            display.fillRect(x + i * 10 - 2, y - 1, 16, 10, WHITE);
            display.setTextColor(BLACK);
        } else {
            display.setTextColor(WHITE);
        }
        display.print(controlKeyboard[i].c_str());
    }
}


void OptionMenu::saveConfiguration() {
    preferences.begin("guidebeeit", false);
    preferences.putString("deviceName", "Harry");
    preferences.putString("channelId", "<1234>");
    preferences.putBool("keyFastSpeed", false);
    preferences.putUChar("syncWord", 0x34);
    preferences.putBool("playSound", true);
    preferences.end();
}

void OptionMenu::drawInputValue(std::string value) {
    int x = OFFSET_X;
    int y = CHAR_HEIGHT + 3;
    display.fillRect(0, y, SCREEN_WIDTH, CHAR_HEIGHT, WHITE);
    display.setCursor(x, y);
    display.setTextColor(BLACK);
    display.print(value.c_str());
    display.setTextColor(WHITE);
}


