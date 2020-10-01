//
// Created by Jing SHEN on 1/10/20.
//

#ifndef ESP32MORSE_BASE_VIEW_HPP
#define ESP32MORSE_BASE_VIEW_HPP

#include "oled_display.hpp"
#include "screen.hpp"
#include "keypad.hpp"

class BaseView : public KeypadListener {

protected:

    OledDisplay display;
    Keypad keypad;
    DynamicScreen topBar;
    unsigned long current_mills;
    unsigned long last_task_mills;
    unsigned long diff_mills;

    std::list<int> topBarPattern = std::list<int>{9500, 500};

public:

    explicit BaseView() : topBar(DynamicScreen(&display, 0, 1, false)) {}

    virtual void setup();

    virtual void loop();


    void onMainPressed() override;

    void onMainReleased() override;

    void onLeftPressed() override;

    void onLeftReleased() override;

    void onRightPressed() override;

    void onRightReleased() override;

    void onOkPressed() override;

    void onOkReleased() override;

};

#endif //ESP32MORSE_BASE_VIEW_HPP
