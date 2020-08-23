//
// Created by Jing SHEN on 21/8/20.
//


#include "morse_code.hpp"


void MorseCodePatternMatch::addListener(MorseCodeListener *listener) {
    morseCodeListener = listener;
}

MorseCodePatternMatch::MorseCodePatternMatch(int dotLimit) {
    decodeDataState = None;
    this->dotLimit = float(dotLimit);
    partLimit = this->dotLimit + this->dotLimit / 2;
    charLimit = this->dotLimit * 3 - this->dotLimit / 2;
    wordLimit = this->dotLimit * 6 - this->dotLimit / 2;
    reset();
}

void MorseCodePatternMatch::changeDotLimit(float value) {
    dotLimit = value;
    partLimit = dotLimit + dotLimit / 2;
    charLimit = dotLimit * 3 - dotLimit / 2;
    wordLimit = dotLimit * 6 - dotLimit / 2;
}

void MorseCodePatternMatch::process(bool isTone) {
    noCharDetectedCounter++;
    if (isTone) {//tone
        spaceCounter = 0;
        sampleCounter++;
        float dotLowerLimit = 3.0;
        if (sampleCounter > dotLowerLimit - 1) {
            if (decodeDataState == None) {
                decodeDataState = Tone;

                if (morseCodeListener != nullptr) {
                    morseCodeListener->onCharStart();
                }
            }
        }
    } else {//none
        switch (decodeDataState) {
            case None: {
                spaceCounter++;
            }
                break;
            case Tone: {
                {
                    estimateWPM(sampleCounter);
                    if (sampleCounter > partLimit) {
                        morseMsg += MC_DASH_SYMBOL;
                        dashLength = sampleCounter;
                        sampleCounter = 0;
                        spaceCounter = 0;
                        decodeDataState = None;
                        if (morseCodeListener != nullptr) {
                            morseCodeListener->onCharEnd(MC_DASH_SYMBOL, (int) dashLength);
                        }
                    } else {
                        if (sampleCounter > partLimit / 2) {
                            morseMsg += MC_DOT_SYMBOL;
                            dotLength = sampleCounter;
                            sampleCounter = 0;
                            spaceCounter = 0;
                            decodeDataState = None;
                            if (morseCodeListener != nullptr) {
                                morseCodeListener->onCharEnd(MC_DOT_SYMBOL, (int) dotLength);
                            }
                        }
                    }

                }

            }
                break;
        }
    }
    if (spaceCounter > charLimit) {
        //can output
        if (morseMsg.length() > 0) {
            if (morseCodeListener != nullptr) {
                morseCodeListener->onEmit(getMorseLetter(morseMsg));
                noCharDetectedCounter = 0;
            }
            reset();
        }
    }

    if (spaceCounter > wordLimit) {
        if (morseCodeListener != nullptr) {
            morseCodeListener->onEmit(' ');
        }
        reset();
    }
    adjustWPM();
}

void MorseCodePatternMatch::adjustWPM() {
    if (dashLength * dotLength > 0) {
        changeDotLimit(fmin(dashLength / 3, dotLength));
    }
}

char MorseCodePatternMatch::getMorseLetter(std::string morseMsg) {
    if (morseCodeData.count(morseMsg) > 0) {
        return morseCodeData[morseMsg];
    }
    return INVALID_SYMBOL;
}

void MorseCodePatternMatch::reset() {
    sampleCounter = 0;
    spaceCounter = 0;
    decodeDataState = None;
    morseMsg = "";
}

void MorseCodePatternMatch::estimateWPM(float newValue) {
    if (estimateDotLength > 0) {
        {
            float value1 = fmin(estimateDotLength, newValue);
            float value2 = fmax(estimateDotLength, newValue);
            if (value2 > value1 * 2.5 && value2 < value1 * 6) {
                changeDotLimit(fmin(value2 / 3, value1));
            }
        }

    }
    estimateDotLength = newValue;
}