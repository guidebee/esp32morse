
#include "morse_walkie_talkie.hpp"
#include "option_menu.hpp"

MorseWalkieTalkie morseWalkieTalkie;
#define POWER_SWITCH_PIN 33
#define BUTTON_PIN_BITMASK 0x200000000 // 2^33 in hex

void setup() {
    int value = digitalRead(POWER_SWITCH_PIN);
    if (value == HIGH) {
        // Serial.print("High\n");
    } else {
        // Serial.print("Low\n");
        esp_deep_sleep_start();
    }
    morseWalkieTalkie.setup();
    pinMode(POWER_SWITCH_PIN, INPUT_PULLDOWN);
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);
}

void loop() {
    int value = digitalRead(POWER_SWITCH_PIN);
    if (value == HIGH) {
        // Serial.print("High\n");
    } else {
        // Serial.print("Low\n");
        esp_deep_sleep_start();
    }
    morseWalkieTalkie.loop();

}