#include <Arduino.h>
#include "../lib/morse/src/morse_code.hpp"
MorseCodePatternMatch morseCode = MorseCodePatternMatch(3);
void setup() {
// write your initialization code here
    Serial.begin(115200);
    char letter=morseCode.getMorseLetter("...");
    Serial.println(letter);

}

void loop() {
// write your code here
}