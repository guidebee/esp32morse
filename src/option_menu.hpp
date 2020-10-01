//
// Created by Jing SHEN on 1/10/20.
//

#ifndef ESP32MORSE_OPTION_MENU_HPP
#define ESP32MORSE_OPTION_MENU_HPP

#include "keypad.hpp"
#include "oled_display.hpp"
#include "screen.hpp"

#include "signal_led.hpp"

#define RECEIVER_LED 12
#define BLUETOOTH_LED 13

class OptionMenu : public KeypadListener {

private:
    OledDisplay display;
    SignalLed receiverLed;
    SignalLed blueToothLed;
    Keypad keypad;
    DynamicScreen topBar;

    unsigned long last_mills;
    unsigned long current_mills;
    unsigned long last_task_mills;
    unsigned long diff_mills;

    std::list<int> topBarPattern = std::list<int>{9500, 500};
public:

    void setup();

    void loop();

    explicit OptionMenu() : receiverLed(SignalLed(RECEIVER_LED)), blueToothLed(SignalLed(BLUETOOTH_LED)),
                            topBar(DynamicScreen(&display, 0, 1, true)) {}

    void onMainPressed() override;

    void onMainReleased() override;

    void onLeftPressed() override;

    void onLeftReleased() override;

    void onRightPressed() override;

    void onRightReleased() override;

    void onOkPressed() override;

    void onOkReleased() override;

};

#endif //ESP32MORSE_OPTION_MENU_HPP
