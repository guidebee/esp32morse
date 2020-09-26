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
            if(led_signal.on){
                digitalWrite(LED_RECEIVER, HIGH);
            }else{
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


void ReceiverLed::signal(bool on,int period) {
    _periods.push_back(LedSignal{on,period});
}

void ReceiverLed::signalMessageReceived(){
    signal(true,100);
    signal(false,100);
    signal(true,100);
    signal(false,100);
}