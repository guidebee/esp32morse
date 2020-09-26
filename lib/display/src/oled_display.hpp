//
// Created by Jing SHEN on 18/9/20.
//

#ifndef ESP32MORSE_OLED_DISPLAY_HPP
#define ESP32MORSE_OLED_DISPLAY_HPP

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

class OledDisplay:public Adafruit_SSD1306 {
private:


public:
    explicit OledDisplay() : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST) {}

    void setup();




};

#endif //ESP32MORSE_OLED_DISPLAY_HPP
