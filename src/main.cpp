
#include "morse_walkie_talkie.hpp"
#include "option_menu.hpp"

MorseWalkieTalkie morseWalkieTalkie;
#define POWER_SWITCH_PIN 33

void setup() {
    morseWalkieTalkie.setup();
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1); //1 = High, 0 = Low
    pinMode(POWER_SWITCH_PIN, INPUT_PULLDOWN);
}

void loop() {
    morseWalkieTalkie.loop();

    int value = digitalRead(POWER_SWITCH_PIN);
    if (value == HIGH) {
        // Serial.print("High\n");
    } else {
        // Serial.print("Low\n");
        esp_deep_sleep_start();
    }
}