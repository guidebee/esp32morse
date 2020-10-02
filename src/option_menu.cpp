//
// Created by Jing SHEN on 1/10/20.
//

#include "option_menu.hpp"
#include "configuration.hpp"

extern bool isOptionMode;
extern Configuration globalConfiguration;
#define DEFAULT_DEVICE_NAME "Morse User"

void OptionMenu::onMainReleased() {

}


void OptionMenu::onLeftReleased() {
    switch (menuOption) {
        case 0:
            handleLetterInputLeft();
            break;

        case 3:
        case 4:
            handleNumberInputLeft();
            break;
        case 1:
        case 2:
        case 5:
            handleToggleLeft();
            break;

        default:
            handleMainMenuLeft();
    }
}

void OptionMenu::onRightReleased() {
    switch (menuOption) {
        case 0:
            handleLetterInputRight();
            break;

        case 3:
        case 4:
            handleNumberInputRight();
            break;
        case 1:
        case 2:
        case 5:
            handleToggleRight();
            break;
        default:
            handleMainMenuRight();
            break;
    }
}


void OptionMenu::onOkReleased() {
    switch (menuOption) {
        case 0:
            handleLetterInputOk();
            break;
        case 3:
        case 4:
            handleNumberInputOk();
            break;
        case 1:
        case 2:
        case 5:
            handleToggleOk();
            break;

        default:
            handleMainMenuOk();
            break;
    }
}

void OptionMenu::setup() {
    BaseView::setup();
    receiverLed.setup();
    blueToothLed.setup();
    blueToothLed.signalMorseText("Menu");
    deviceName = globalConfiguration.deviceName;
    playSound = globalConfiguration.playSound;
    bluetooth = globalConfiguration.bluetooth;
    inputSpeed = globalConfiguration.inputSpeed;
    channelId = globalConfiguration.channelIdValue;
    syncWord = globalConfiguration.syncWord;

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

            drawInputValue(deviceName);
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


        case 5:
            options[0] = "Fast";
            options[1] = "Normal";
            extraOffsetY = CHAR_HEIGHT;
            upperMenuIndex = 1;
            drawOptionMenus(extraOffsetY);
            break;
        case 3:
            sprintf(buffer, "%04d", channelId);
            drawInputValue(buffer);
            drawNumberKeyboard();
            drawControlKeyboard();
            break;
        case 4: {

            sprintf(buffer, "%d", syncWord);
            drawInputValue(buffer);
            drawNumberKeyboard();
            drawControlKeyboard();
        }
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
        display.setCursor(x + col * 10 + 1, y + row * 10);
        display.print(fullKeyboard[i]);
    }

}

void OptionMenu::drawNumberKeyboard() {
    int x = OFFSET_X;
    int y = CHAR_HEIGHT * 5;

    for (int i = 0; i < 12; i++) {

        if (numberSelected == i) {
            display.fillRect(x + i * 10 - 2, y - 1, 10, 10, WHITE);
            display.setTextColor(BLACK);
        } else {
            display.setTextColor(WHITE);
        }
        display.setCursor(x + i * 10, y);
        display.print(numberKeyboard[i]);
    }
}

void OptionMenu::drawControlKeyboard() {
    int x = OFFSET_X;
    int y = CHAR_HEIGHT * 3 + 30;
    for (int i = 0; i < 4; i++) {
        display.setCursor(x + i * 20, y);
        if (controlSelected == i) {
            display.fillRect(x + i * 20 - 2, y - 1, 16, 10, WHITE);
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
    preferences.putBool("inputSpeed", false);
    preferences.putUChar("syncWord", 0x34);
    preferences.putBool("playSound", true);
    preferences.end();
}

void OptionMenu::drawInputValue(std::string value) {
    int x = OFFSET_X;
    int y = CHAR_HEIGHT + 3;
    display.fillRect(0, y - 1, SCREEN_WIDTH, CHAR_HEIGHT + 2, WHITE);
    display.setCursor(x, y);
    display.setTextColor(BLACK);
    display.print(value.c_str());
    display.setTextColor(WHITE);
}

void OptionMenu::handleToggleLeft() {
    handleMainMenuLeft();
}

void OptionMenu::handleToggleRight() {
    handleMainMenuRight();
}

void OptionMenu::handleToggleOk() {
    //save boolean options
    switch (menuOption) {
        case 1: //play sound
            playSound = currentSelect == 0;
            globalConfiguration.playSound = playSound;
            preferences.begin("guidebeeit", false);
            preferences.putBool("playSound", playSound);
            preferences.end();
            break;
        case 2: //bluetooth
            bluetooth = currentSelect == 0;
            globalConfiguration.bluetooth = bluetooth;
            preferences.begin("guidebeeit", false);
            preferences.putBool("bluetooth", bluetooth);
            preferences.end();
            break;
        case 5:

            inputSpeed = currentSelect == 0;
            globalConfiguration.inputSpeed = inputSpeed;
            preferences.begin("guidebeeit", false);
            preferences.putBool("inputSpeed", inputSpeed);
            preferences.end();
            break;

    }
    menuOption = -1;
    drawClientArea();
}

void OptionMenu::handleLetterInputLeft() {
    if (letterSelected >= 0) {
        letterSelected -= 1;
        if (letterSelected < 0) {
            letterSelected = -1;
            controlSelected = 3;
        } else {
            controlSelected = -1;
        }
    } else {
        controlSelected -= 1;
        if (controlSelected < 0) {
            controlSelected = -1;
            letterSelected = 35;
        }
    }
    drawClientArea();

}

void OptionMenu::handleLetterInputRight() {
    if (letterSelected >= 0) {
        letterSelected += 1;
        if (letterSelected > 35) {
            letterSelected = -1;
            controlSelected = 0;
        } else {
            controlSelected = -1;
        }
    } else {
        controlSelected += 1;
        if (controlSelected > 3) {
            controlSelected = -1;
            letterSelected = 0;
        }
    }
    drawClientArea();
}

void OptionMenu::handleLetterInputOk() {
    char buffer[64];
    switch (menuOption) {
        case 0:
            if (letterSelected >= 0) {
                deviceName += fullKeyboard[letterSelected];
            } else {
                switch (controlSelected) {
                    case 0:
                        deviceName += ' ';
                        break;
                    case 1:
                        deviceName = deviceName.substr(0, deviceName.length() - 1);
                        break;
                    case 2:
                        //save device name
                        if (deviceName.empty()) {
                            deviceName = DEFAULT_DEVICE_NAME;
                        }
                        deviceName[0] = toupper(deviceName[0]);
                        globalConfiguration.deviceName = deviceName;
                        preferences.begin("guidebeeit", false);
                        preferences.putString("deviceName", deviceName.c_str());
                        preferences.end();
                        menuOption = -1;
                        drawClientArea();
                        break;
                    case 3:
                        deviceName = "";
                        break;

                }
            }
            break;

        case 3: //channel id
            if (numberSelected >= 0) {
                sprintf(buffer, "%04d", channelId);
                std::string channelIdString = buffer;
                channelIdString += numberKeyboard[numberSelected];
                channelId = std::atoi(channelIdString.c_str());
                channelId = channelId % 10000;
            } else {
                switch (controlSelected) {
                    case 0:

                        break;
                    case 1: {
                        sprintf(buffer, "%04d", channelId);
                        std::string channelIdString = buffer;
                        channelIdString = channelIdString.substr(0, channelIdString.length() - 1);
                        if (!channelIdString.empty()) {
                            channelId = std::atoi(channelIdString.c_str());
                        } else {
                            channelId = 0;
                        }

                    }
                        break;
                    case 2:
                        //save device name
                        if (channelId == 0) {
                            channelId = 1234;
                        }
                        globalConfiguration.channelIdValue = channelId;
                        sprintf(buffer, "<%04d>", channelId);
                        globalConfiguration.channelId = buffer;
                        preferences.begin("guidebeeit", false);
                        preferences.putString("channelId", globalConfiguration.channelId.c_str());
                        preferences.putInt("channelIdValue", channelId);
                        preferences.end();
                        menuOption = -1;
                        drawClientArea();
                        break;
                    case 3:
                        channelId = 1234;
                        break;

                }
            }
            break;
        case 4:
            if (numberSelected >= 0) {
                sprintf(buffer, "%d", syncWord);
                std::string syncWordString = buffer;
                syncWordString += numberKeyboard[numberSelected];
                syncWord = std::atoi(syncWordString.c_str());
                syncWord = syncWord % 255;
            } else {
                switch (controlSelected) {
                    case 0:

                        break;
                    case 1: {
                        sprintf(buffer, "%d", syncWord);
                        std::string syncWordString = buffer;
                        syncWordString = syncWordString.substr(0, syncWordString.length() - 1);
                        if (!syncWordString.empty()) {
                            syncWord = std::atoi(syncWordString.c_str());
                        } else {
                            syncWord = 0;
                        }
                        syncWord = syncWord % 255;
                    }
                        break;
                    case 2:
                        //save device name
                        if (syncWord == 0) {
                            channelId = 52;
                        }
                        globalConfiguration.syncWord = syncWord;

                        preferences.begin("guidebeeit", false);
                        preferences.putUChar("syncWord", globalConfiguration.syncWord);
                        preferences.end();
                        menuOption = -1;
                        drawClientArea();
                        break;
                    case 3:
                        syncWord = 52;
                        break;

                }
            }
            break;;
        default:
            break;

    }

    drawClientArea();
}

void OptionMenu::handleNumberInputLeft() {
    if (numberSelected >= 0) {
        numberSelected -= 1;
        if (numberSelected < 0) {
            numberSelected = -1;
            controlSelected = 3;
        } else {
            controlSelected = -1;
        }
    } else {
        controlSelected -= 1;
        if (controlSelected < 0) {
            controlSelected = -1;
            numberSelected = 11;
        }
    }
    drawClientArea();
}

void OptionMenu::handleNumberInputRight() {
    if (numberSelected >= 0) {
        numberSelected += 1;
        if (numberSelected > 11) {
            numberSelected = -1;
            controlSelected = 0;
        } else {
            controlSelected = -1;
        }
    } else {
        controlSelected += 1;
        if (controlSelected > 3) {
            controlSelected = -1;
            numberSelected = 0;
        }
    }
    drawClientArea();
}

void OptionMenu::handleNumberInputOk() {
    handleLetterInputOk();
}

void OptionMenu::handleMainMenuLeft() {
    currentSelect -= 1;
    if (currentSelect < 0) currentSelect = upperMenuIndex;
    drawClientArea();
}

void OptionMenu::handleMainMenuRight() {
    currentSelect += 1;
    if (currentSelect > upperMenuIndex) currentSelect = 0;
    drawClientArea();
}

void OptionMenu::handleMainMenuOk() {
    switch (currentSelect) {
        case 0:
            topBar.displayText("   Device Name", topBarPattern);
            menuOption = 0;
            drawClientArea();
            break;
        case 1:
            menuOption = 1;
            topBar.displayText("    Play Sound", topBarPattern);
            if (playSound) {
                currentSelect = 0;
            } else {
                currentSelect = 1;
            }
            drawClientArea();
            break;
        case 2:
            menuOption = 2;
            topBar.displayText("    Bluetooth", topBarPattern);
            if (bluetooth) {
                currentSelect = 0;
            } else {
                currentSelect = 1;
            }
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
            menuOption = 5;
            if (inputSpeed) {
                currentSelect = 0;
            } else {
                currentSelect = 1;
            }
            drawClientArea();
            break;
        case 6:
            isOptionMode = false;
            break;
    }
}


