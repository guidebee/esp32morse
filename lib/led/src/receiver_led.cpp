//
// Created by Jing SHEN on 26/9/20.
//

#include "receiver_led.hpp"


//
// Created by Jing SHEN on 18/9/20.
//

#include "buzzer_tone.hpp"

ReceiverLed::ReceiverLed() {
    _last_time = millis();
    _remaining_period = 0;
}

void ReceiverLed::setup() {
    pinMode(LED_RECEIVER, OUTPUT);
}

void ReceiverLed::loop() {
    auto current_mills = millis();
    _remaining_period -= static_cast<int>(current_mills - _last_time);
    if (_remaining_period < 10) {
        if (!_periods.empty()) {
            auto led_signal = _periods.front();
            _periods.pop_front();
            if (led_signal.on) {
                digitalWrite(LED_RECEIVER, HIGH);
            } else {
                digitalWrite(LED_RECEIVER, LOW);
            }
            _remaining_period = led_signal.period;
        } else {
            stop();
        }
    }
    _last_time = current_mills;
    if (_remaining_period < 0) _remaining_period = 0;
}

void ReceiverLed::stop() {
    digitalWrite(LED_RECEIVER, LOW);
    _periods.clear();
}


void ReceiverLed::signal(bool on, int period) {
    _periods.push_back(LedSignal{on, period});
}


void ReceiverLed::signalMorse(std::string rawCode) {

    if (rawCode != " ") {
        for (char &c : rawCode) {
            if (c == '.') {
                signal(true, tonePeriod );
            } else {
                signal(true, tonePeriod * 3);
            }
            signal(false, tonePeriod );
        }
    }

}


void ReceiverLed::signalMorseText(std::string text) {
    if (text != " ") {
        for (char &c : text) {
            switch (c) {
                case '.':
                    signal(true, tonePeriod);
                    break;
                case '-':
                    signal(true, tonePeriod * 3);
                case '<':
                    signal(false, tonePeriod * 3);
                    break;
                case '^':
                    signal(false, tonePeriod);
                    break;
                case '>':
                    signal(false, tonePeriod * 7);
                    break;
                default:
                    break;

            }

        }
    }
}


void ReceiverLed::signalMessageReceived() {
    signal(true, 100);
    signal(false, 100);
    signal(true, 100);
    signal(false, 100);
}

void ReceiverLed::signalMessageSent() {
    signal(true, 200);
    signal(false, 100);
    signal(true, 2);
    signal(false, 100);
}