//
// Created by Jing SHEN on 18/9/20.
//

#include "oled_display.hpp"

void OledDisplay::init() {
    //reset OLED display via software
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, LOW);
    delay(20);
    digitalWrite(OLED_RST, HIGH);
    //initialize OLED
    Wire.begin(OLED_SDA, OLED_SCL);
    if (!begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Don't proceed, loop forever
    }

}