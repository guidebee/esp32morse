//
// Created by Jing SHEN on 1/10/20.
//

#ifndef ESP32MORSE_OPTION_MENU_HPP
#define ESP32MORSE_OPTION_MENU_HPP

#include <Preferences.h>
#include "keypad.hpp"
#include "oled_display.hpp"
#include "screen.hpp"
#include "base_view.hpp"

#include "signal_led.hpp"
#include <cstring>

#define RECEIVER_LED 12
#define BLUETOOTH_LED 13


class OptionMenu : public BaseView {

private:

    SignalLed receiverLed;
    SignalLed blueToothLed;

    int currentSelect = 0;
    int menuOption = -1;
    std::string options[7];

    int upperMenuIndex = 6;

    char fullKeyboard[36] = {'a', 'b', 'c', 'd', 'e', 'f', 'g',
                             'h', 'i', 'j', 'k', 'l', 'm', 'n',
                             'o', 'p', 'q', 'r', 's', 't', 'u',
                             'v', 'w', 'x', 'y', 'z', '0', '1',
                             '2', '3', '4', '5', '6', '7', '8',
                             '9'};

    char numberKeyboard[12] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1'};

    std::string controlKeyboard[4] = {"sp", "bs", "ok","cl"};

    std::string deviceName;
    bool playSound;
    bool bluetooth;
    int channelId;
    int syncWord;
    bool inputSpeed;
    Preferences preferences;

    int numberSelected = 0;
    int letterSelected = 0;
    int controlSelected = -1;


public:

    void setup() override;

    void loop() override;


    explicit OptionMenu() : receiverLed(SignalLed(RECEIVER_LED)),
                            blueToothLed(SignalLed(BLUETOOTH_LED)) {
        options[0] = "Device Name";
        options[1] = "Play Sound";
        options[2] = "Bluetooth";
        options[3] = "Channel Id";
        options[4] = "Sync Word";
        options[5] = "Input Speed";
        options[6] = "Exit";

    }

    void drawClientArea();

    void onMainReleased() override;

    void onLeftReleased() override;

    void onRightReleased() override;

    void onOkReleased() override;

private:


    void drawOptionMenus(int extraOffsetY);

    void drawFullKeyboard();

    void drawNumberKeyboard();

    void drawControlKeyboard();

    void saveConfiguration();

    void drawInputValue(std::string value);

    void handleToggleLeft();

    void handleToggleRight();

    void handleToggleOk();

    void handleLetterInputLeft();

    void handleLetterInputRight();

    void handleLetterInputOk();

    void handleNumberInputLeft();

    void handleNumberInputRight();

    void handleNumberInputOk();

    void handleMainMenuLeft();

    void handleMainMenuRight();

    void handleMainMenuOk();

};

#endif //ESP32MORSE_OPTION_MENU_HPP
