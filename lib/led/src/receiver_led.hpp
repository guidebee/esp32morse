//
// Created by Jing SHEN on 26/9/20.
//

#ifndef ESP32MORSE_RECEIVER_LED_HPP
#define ESP32MORSE_RECEIVER_LED_HPP

#define LED_RECEIVER 12
#include <string>
#include <list>

struct LedSignal{
    bool on;
    int period;
};

class ReceiverLed {
private:
    const int tonePeriod=100;
    unsigned long _last_time;
    int _remaining_period;
    std::list<LedSignal> _periods = std::list<LedSignal>();

public:
    explicit ReceiverLed();

    void setup();

    void stop();

    void loop();

    void signal(bool on,int period);

    void signalMorseText(std::string text);

    void signalMessageReceived();

    void signalMorse(std::string raw);

    void signalMessageSent();
};

#endif //ESP32MORSE_RECEIVER_LED_HPP
