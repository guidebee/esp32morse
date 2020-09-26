//
// Created by Jing SHEN on 26/9/20.
//

#ifndef ESP32MORSE_RECEIVER_LED_HPP
#define ESP32MORSE_RECEIVER_LED_HPP

#define LED_RECEIVER 12

#include <list>

struct LedSignal{
    bool on;
    int period;
};

class ReceiverLed {
private:

    unsigned long _last_time;
    int _remaining_period;
    std::list<LedSignal> _periods = std::list<LedSignal>();

public:
    explicit ReceiverLed();

    void setup();

    void stop();

    void loop();

    void signal(bool on,int period);

    void signalMessageReceived();
};

#endif //ESP32MORSE_RECEIVER_LED_HPP
