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

void BuzzerTone::playMessageReceived() {
    playDefaultTone(100);
    playTone(1000, 100);
    playDefaultTone(100);
    playTone(1000, 100);
}

void BuzzerTone::playMessageSent() {

    playNote(NOTE_C,4,200);
    playTone(0,100);
    playNote(NOTE_D,4,200);
    playTone(0,100);
    playNote(NOTE_E,4,200);
    playTone(0,100);



}

void BuzzerTone::playNote(note_t note,uint8_t octave,int period){
    const uint16_t noteFrequencyBase[12] = {
            //   C        C#       D        Eb       E        F       F#        G       G#        A       Bb        B
            4186,    4435,    4699,    4978,    5274,    5588,    5920,    6272,    6645,    7040,    7459,    7902
    };

    if(octave > 8 || note >= NOTE_MAX){
        return;
    }
    double noteFreq =  (double)noteFrequencyBase[note] / (double)(1 << (8-octave));
    playTone(static_cast<int>(noteFreq),period);
}

void BuzzerTone::playMorse(std::string rawCode) {

    if (rawCode != " ") {
        for (char &c : rawCode) {
            if (c == '.') {
                playTone(toneFrequency, tonePeriod);
            } else {
                playTone(toneFrequency, tonePeriod * 3);
            }
            playTone(0, tonePeriod );
        }
    }

}


void BuzzerTone::playMorseText(std::string text) {

    if (text != " ") {
        for (char &c : text) {
            switch (c) {
                case '.':
                    playTone(toneFrequency, tonePeriod);
                    break;
                case '-':
                    playTone(toneFrequency, tonePeriod * 3);
                case '<':
                    playTone(0, tonePeriod*3);
                    break;
                case '^':
                    playTone(0, tonePeriod);
                    break;
                case '>':
                    playTone(0, tonePeriod *7);
                    break;
                default:
                    break;

            }

        }
    }

}



void BuzzerTone::playDi() {
    stop();
    playTone(keyToneFrequency, keyTonePeriod);


}

void BuzzerTone::playDah() {
    stop();
    playTone(keyToneFrequency, keyTonePeriod * 3);
}