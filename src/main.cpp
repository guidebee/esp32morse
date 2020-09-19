#include <Arduino.h>
#include <ArduinoLog.h>

#include "morse_code.hpp"
#include "oled_display.hpp"
#include "lora_radio.hpp"
#include "push_button.hpp"
#include "buzzer_tone.hpp"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include <EEPROM.h>

#define EEPROM_SIZE 1

#define LED_RECEIVER 12
#define TONE_PERIOD 200
OledDisplay display;
BuzzerTone buzzer;
LoraRadioClass LoRaRadio;
KeyboardMorseCodeDecoder morseCode = KeyboardMorseCodeDecoder();
PushButton button(36);
uint64_t chipid;
bool isDown = true;
String message="";
char lastChar='^';
class KeyListener : public MorseCodeListener {
    void onEmit(char character) {

        //Send LoRaRadio packet to receiver
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
            LoRaRadio.beginPacket();
            LoRaRadio.print(message);
            LoRaRadio.endPacket();
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
String LoRaData;
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

    while(!Serial && !Serial.available()){}
    randomSeed(analogRead(0));
    // Pass log level, whether to show log level, and print interface.
    // Available levels are:
    // LOG_LEVEL_SILENT, LOG_LEVEL_FATAL, LOG_LEVEL_ERROR, LOG_LEVEL_WARNING, LOG_LEVEL_NOTICE, LOG_LEVEL_TRACE, LOG_LEVEL_VERBOSE
    // Note: if you want to fully remove all logging code, uncomment #define DISABLE_LOGGING in Logging.h
    //       this will significantly reduce your project size

    Log.begin(LOG_LEVEL_VERBOSE, &Serial);

    display.init();
    buzzer.init();
    ledcWriteTone(channel,2000);
    LoRaRadio.init();
    Log.notice ("LoRaRadio Initializing OK!");
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("morse walkie talkie  ");
//    for(int k=0;k<15;k++) {
//        for (int i = 0; i < 26; i++) {
//            display.print((char)(i+'A'));
//        }
//    }
    display.display();

    button.setDebounceTime(50);
    morseCode.addListener(&keyListener);
    char letter = morseCode.getMorseLetter("...");

    Serial.println(letter);
    chipid = ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
    Log.notice("ESP32 Chip ID = %04X", (uint16_t) (chipid >> 32));//print High 2 bytes
    Log.notice ("%08X\n", (uint32_t) chipid);//print Low 4bytes.
    last_mills=millis();
    current_mills=last_mills;
    last_task_mills=last_mills;
    diff_mills=0;
    samplePeriod=morseCode.getSamplePeriod();
    display.setCursor(0,10);
    ledcWriteTone(channel,0);
    pinMode(LED_RECEIVER, OUTPUT);

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

        isDown=true;
//        Serial.println("The button is pressed");
        morseCode.setOnOff(true);
    }

//
//
    if (button.isReleased()) {
        isDown=false;

        diff_mills=millis()-last_mills;


//        Serial.println("The button is released");
        if (diff_mills < TONE_PERIOD){

            morseCode.processKey(false);
        }else if(diff_mills<TONE_PERIOD*4){

            morseCode.processKey(true);
        }

        morseCode.setOnOff(false);
    }

    int packetSize = LoRaRadio.parsePacket();
    if (packetSize) {
        //received a packet
        Serial.print("Received packet ");

        //read packet
        while (LoRaRadio.available()) {
            LoRaData = LoRaRadio.readString();
            Serial.print(LoRaData);
        }
        display.print(LoRaData);
        display.display();
        ledcWriteTone(channel,2000);
        digitalWrite(LED_RECEIVER, HIGH);
        delay(100);
        ledcWriteTone(channel,1000);
        digitalWrite(LED_RECEIVER, LOW);
        delay(100);
        digitalWrite(LED_RECEIVER, HIGH);
        ledcWriteTone(channel,2000);
        delay(100);
        digitalWrite(LED_RECEIVER, LOW);
        ledcWriteTone(channel,0);
    }

}