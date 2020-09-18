#include <Arduino.h>
#include "morse_code.hpp"
#include "oled_display.hpp"
#include <ezButton.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <LoRa.h>
#include <EEPROM.h>
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
#define BAND 915E6
#define EEPROM_SIZE 1

#define LED_PIN 25
#define TONE_PERIOD 200
OledDisplay display;
KeyboardMorseCodeDecoder morseCode = KeyboardMorseCodeDecoder();
ezButton button(36);
uint64_t chipid;
bool isDown = true;
String message="";
char lastChar='^';
class KeyListener : public MorseCodeListener {
    void onEmit(char character) {

        //Send LoRa packet to receiver
        if(!(character==' ' and lastChar==' ')) {
            if(character!='*') {
                message += character;
                display.print(character);
                Serial.print(character);
                display.display();
            }
        }
        lastChar=character;
        if(character=='*') {
            LoRa.beginPacket();
            LoRa.print(message);
            LoRa.endPacket();
            message="";
            display.setCursor(0,10);
            display.clearDisplay();

        }
    }

    void onCharStart() {
//        Serial.print("onCharStart");
    }

    void onCharEnd(char dotOrDash, int length) {
//        display.print(dotOrDash);
        Serial.print(dotOrDash);
//        display.display();
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
    printf("Hello world!\n");
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
           "esp32",
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
    Serial.begin(115200);
    display.init();
    //setup LoRa transceiver module
    LoRa.setPins(SS, RST, DIO0);

    //SPI LoRa pins
    SPI.begin(SCK, MISO, MOSI, SS);
    //setup LoRa transceiver module
    LoRa.setPins(SS, RST, DIO0);

    if (!LoRa.begin(BAND)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    LoRa.setSyncWord(0x34);
    Serial.println("LoRa Initializing OK!");
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
        if (diff_mills < TONE_PERIOD){

            morseCode.processKey(false);
        }else if(diff_mills<TONE_PERIOD*4){

            morseCode.processKey(true);
        }

        morseCode.setOnOff(false);
    }


}