//
// Created by Jing SHEN on 21/8/20.
//

#ifndef ARDUINOMORSE_MORSE_CODE_HPP
#define ARDUINOMORSE_MORSE_CODE_HPP

#include <string>
#include <map>
#include <cmath>

class MorseCodePatternMatch {

    enum Status {
        None,
        Tone
    };

    class MorseCodeListener {
    public:
        virtual void onEmit(char character) = 0;

        virtual void onCharStart() = 0;

        virtual void onCharEnd(char dotOrDash, int length) = 0;
    };


public:

    void addListener(MorseCodeListener *listener);

    explicit MorseCodePatternMatch(int dotLimit);

    void changeDotLimit(float value);

    void process(bool isTone);

    char getMorseLetter(std::string morseMsg);

private:

    void adjustWPM();



    void reset();

    void estimateWPM(float newValue);

protected:
    int noCharDetectedCounter = 0;

private:
    float sampleCounter = 0;
    float spaceCounter = 0;
    std::string morseMsg;

    float dotLimit = 3.0;
    float partLimit = dotLimit;
    float charLimit = dotLimit * 3;
    float wordLimit = dotLimit * 7;
    float dotLength = 0;
    float dashLength = 0;
    float estimateDotLength = 0;

private:
    const char MC_DOT_SYMBOL = '.';
    const char MC_DASH_SYMBOL = '-';
    const char INVALID_SYMBOL = '^';

    MorseCodeListener *morseCodeListener = nullptr;

    std::map<std::string, char> morseCodeData =
            {{".-",      'a'},
             {"-...",    'b'},
             {"-.-.",    'c'},
             {"-..",     'd'},
             {".",       'e'},
             {"..-.",    'f'},
             {"--.",     'g'},
             {"....",    'h'},
             {"..",      'i'},
             {".---",    'j'},
             {"-.-",     'k'},
             {".-..",    'l'},
             {"--",      'm'},
             {"-.",      'n'},
             {"---",     'o'},
             {".--.",    'p'},
             {"--.-",    'q'},
             {".-.",     'r'},
             {"...",     's'},
             {"-",       't'},
             {"..-",     'u'},
             {"...-",    'v'},
             {".--",     'w'},
             {"-..-",    'x'},
             {"-.--",    'y'},
             {"--..",    'z'},

             {"-----",   '0'},
             {".----",   '1'},
             {"..---",   '2'},
             {"...--",   '3'},
             {"....-",   '4'},
             {".....",   '5'},
             {"-....",   '6'},
             {"--...",   '7'},
             {"---..",   '8'},
             {"----.",   '9'},


             {".-.-.-",  '.'},
             {"--..--",  ','},
             {"..--..",  '?'},
             {".----.",  '\''},
             {"-.-.--",  '!'},
             {"-..-.",   '/'},
             {"-.--.",   '('},
             {"-.--.-",  ')'},
             {".-...",   '&'},
             {"---...",  ':'},
             {"-.-.-.",  ';'},
             {"-...-",   '='},
             {".-.-.",   '+'},
             {"-....-",  '-'},
             {"..--.-",  '_'},
             {".-..-.",  '\"'},
             {"...-..-", '$'},
             {".--.-.",  '@'},
             {".-.-.",   '\t'},
             {".-.-",    '\n'}};

private:
    Status decodeDataState;

};


class KeyboardMorseCodeDecoder : MorseCodePatternMatch {

public:
    KeyboardMorseCodeDecoder(int dotLimit, int samplePeriod)
            : MorseCodePatternMatch(dotLimit) {
        this->samplePeriod = samplePeriod;

    }

    KeyboardMorseCodeDecoder() : KeyboardMorseCodeDecoder(18, 25) {}

    void processKey(bool input) {
        int factor = 15;
        if (!input) {
            for (int i = 0; i < factor; i++) {
                process(true);
            }
            for (int i = 0; i < factor; i++) {
                process(false);
            }
        } else {
            for (int i = 0; i < factor * 3; i++) {
                process(true);
            }
            for (int i = 0; i < factor; i++) {
                process(false);
            }
        }

    }

    void setSampleSpeed(int newSpeed) {
        samplePeriod = newSpeed;
    }

    void setOnOff(bool isOnOff) {
        isOn = isOnOff;
    }

private:
    volatile bool isOn = false;
    int samplePeriod = 25;
};

#endif //ARDUINOMORSE_MORSE_CODE_HPP
