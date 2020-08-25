#include <Arduino.h>
#include "morse_code.hpp"
#include <ezButton.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <EEPROM.h>
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

#define EEPROM_SIZE 1
//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define LED_PIN 25
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
KeyboardMorseCodeDecoder morseCode = KeyboardMorseCodeDecoder();
ezButton button(36);
uint64_t chipid;
bool isDown = true;
class KeyListener : public MorseCodeListener {
    void onEmit(char character) {
        display.print(character);
        Serial.print(character);
        display.display();
    }

    void onCharStart() {
//        Serial.print("onCharStart");
    }

    void onCharEnd(char dotOrDash, int length) {
        display.print(dotOrDash);
        Serial.print(dotOrDash);
        display.display();
    }
};

KeyListener keyListener;
unsigned long last_mills;
unsigned long current_mills;
unsigned long last_task_mills;
unsigned long diff_mills;
int samplePeriod;
void setup() {
// write your initialization code here
    Serial.begin(115200);
    //reset OLED display via software
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, LOW);
    delay(20);
    digitalWrite(OLED_RST, HIGH);
    //initialize OLED
    Wire.begin(OLED_SDA, OLED_SCL);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("morse walkie talkie  ");
    display.display();
    pinMode(LED_PIN, OUTPUT);
    button.setDebounceTime(50);
    morseCode.addListener(&keyListener);
    char letter = morseCode.getMorseLetter("...");

    Serial.println(letter);
    chipid = ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
    Serial.printf("ESP32 Chip ID = %04X", (uint16_t) (chipid >> 32));//print High 2 bytes
    Serial.printf("%08X\n", (uint32_t) chipid);//print Low 4bytes.
    last_mills=millis();
    current_mills=last_mills;
    last_task_mills=last_mills;
    diff_mills=0;
    samplePeriod=morseCode.getSamplePeriod();
    display.setCursor(0,10);

}

void loop() {
// write your code here
    button.loop();

    current_mills=millis();
    unsigned long diff_task_mills=current_mills-last_task_mills;
    if(diff_task_mills>=samplePeriod){
        if(!isDown) {
            morseCode.process(false);
        }

        last_task_mills=current_mills;
    }

    if (button.isPressed()) {
        last_mills=millis();
        digitalWrite(LED_PIN, HIGH);
        isDown=true;
//        Serial.println("The button is pressed");
        morseCode.setOnOff(true);
    }

//
//
    if (button.isReleased()) {
        isDown=false;

        diff_mills=millis()-last_mills;

        digitalWrite(LED_PIN, LOW);
//        Serial.println("The button is released");
        if (diff_mills < 250){

            morseCode.processKey(false);
        }else if(diff_mills<1000){

            morseCode.processKey(true);
        }

        morseCode.setOnOff(false);
    }


}