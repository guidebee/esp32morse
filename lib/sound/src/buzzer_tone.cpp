//
// Created by Jing SHEN on 18/9/20.
//

#include "buzzer_tone.hpp"

BuzzerTone::BuzzerTone() {
    _last_time = millis();
    _remaining_period = 0;
}

void BuzzerTone::setup() {
    int channel = 0;
    int resolution = 8;
    ledcSetup(channel, default_frequency, resolution);
    ledcAttachPin(21, channel);
}

void BuzzerTone::loop() {
    auto current_mills = millis();
    _remaining_period -= static_cast<int>(current_mills - _last_time);
    if (_remaining_period < 10) {
        if (!_tones.empty()) {
            auto tone = _tones.front();
            _tones.pop_front();
            ledcWriteTone(0, tone.frequency);
            _remaining_period = tone.period;
        } else {
            stop();
        }
    }
    _last_time = current_mills;
    if (_remaining_period < 0) _remaining_period = 0;
}

void BuzzerTone::stop() {
    ledcWriteTone(0, 0);
    _tones.clear();
}

void BuzzerTone::playTone(int freq, int period) {
    _tones.push_back(Tone{freq, period});
}

void BuzzerTone::playDefaultTone(int period) {
    _tones.push_back(Tone{default_frequency, period});
}

void BuzzerTone::playMessageReceived(){
    playDefaultTone(100);
    playTone(1000, 100);
    playDefaultTone(100);
    playTone(1000, 100);
}