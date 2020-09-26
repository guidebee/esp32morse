//
// Created by Jing SHEN on 18/9/20.
//

#ifndef ESP32MORSE_BUZZER_TONE_HPP
#define ESP32MORSE_BUZZER_TONE_HPP

#include <Arduino.h>
#include <list>



struct Tone{
    int frequency;
    int period;
};

class BuzzerTone {

private:
    const int tonePeriod=100;
    const int keyTonePeriod=100;
    const int keyToneFrequency=800;
    const int toneFrequency=800;
    const int default_frequency=2000;
    unsigned long _last_time;
    int _remaining_period;
    std::list<Tone> _tones=std::list<Tone>();


public:
    explicit BuzzerTone();

    void setup();

    void stop();

    void loop();

    void playTone(int freq,int period);

    void playDefaultTone(int period);

    void playMessageReceived();

    void playMessageSent();

    void playDi();

    void playDah();

    void playNote(note_t note,uint8_t octave,int period);

    void playMorse(std::string rawCode);

    void playMorseText(std::string text);
};

#endif //ESP32MORSE_BUZZER_TONE_HPP
