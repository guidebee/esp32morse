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

#define RECEIVER_LED 12
#define BLUETOOTH_LED 13

class OptionMenu : public BaseView {

private:

    SignalLed receiverLed;
    SignalLed blueToothLed;



public:

    void setup();

    void loop();

    explicit OptionMenu() : receiverLed(SignalLed(RECEIVER_LED)),
                            blueToothLed(SignalLed(BLUETOOTH_LED)) {}


    void onMainReleased() override;


    void onLeftReleased() override;


    void onRightReleased() override;


    void onOkReleased() override;

};

#endif //ESP32MORSE_OPTION_MENU_HPP
