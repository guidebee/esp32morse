//
// Created by Jing SHEN on 26/9/20.
//

#ifndef ESP32MORSE_RECEIVER_LED_HPP
#define ESP32MORSE_RECEIVER_LED_HPP


#include <string>
#include <list>

struct LedSignal{
    bool on;
    int period;
};

class SignalLed {
private:
    int _led_pin=12;
    const int tonePeriod=100;
    unsigned long _last_time;
    int _remaining_period;
    std::list<LedSignal> _periods = std::list<LedSignal>();

public:
    explicit SignalLed(int pin);

    void setup();

    void stop();

    void loop();

    void signal(bool on,int period);

    void signalMorseText(std::string text);

    void signalMessageReceived();

    void signalError();

    void signalMorse(std::string raw);

    void signalMessageSent();
};

#endif //ESP32MORSE_RECEIVER_LED_HPP
