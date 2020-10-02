//
// Created by Jing SHEN on 1/10/20.
//

#ifndef ESP32MORSE_OPTION_MENU_HPP
#define ESP32MORSE_OPTION_MENU_HPP

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

    int currentSelect = 6;
    int menuOption = -1;
    std::string options[7];

    int upperMenuIndex = 6;


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
    void onChooseOn();

    void onChooseOff();

    void drawOptionMenus(int extraOffsetY);

};

#endif //ESP32MORSE_OPTION_MENU_HPP
