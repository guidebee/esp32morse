
#include "morse_walkie_talkie.hpp"
#include "option_menu.hpp"

volatile bool isOptionMode = false;
MorseWalkieTalkie morseWalkieTalkie;
OptionMenu optionMenu;


void setup() {
    morseWalkieTalkie.setup();
    optionMenu.setup();
}

void loop() {
    if (isOptionMode) {
        optionMenu.loop();
    } else {
        morseWalkieTalkie.loop();
    }
}