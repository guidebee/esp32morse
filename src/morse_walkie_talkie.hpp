//
// Created by Jing SHEN on 27/9/20.
//

#ifndef ESP32MORSE_MORSE_WALKIE_TALKIE_HPP
#define ESP32MORSE_MORSE_WALKIE_TALKIE_HPP

#include <Arduino.h>
#include <ArduinoLog.h>
#include <map>
#include <Preferences.h>
#include "morse_code.hpp"
#include "oled_display.hpp"
#include "screen.hpp"
#include "lora_radio.hpp"
#include "buzzer_tone.hpp"
#include "signal_led.hpp"
#include "keypad.hpp"
#include "option_menu.hpp"

#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"


#define RECEIVER_LED 12
#define BLUETOOTH_LED 13

#define ENTER_MENU_COUNT 3

const bool startLow = true;
const bool supportSleep = false;
const bool needCheckDevice = false;

struct UserInfo {
    int index;
    std::string deviceName;
    int counter;
};


class MorseWalkieTalkie : public MorseCodeListener,
                          public KeypadListener,
                          public LoraMessageListener,
                          public ExtraDisplayListener {
private:
    unsigned long last_mills;
    unsigned long current_mills;

    unsigned long last_top_bar_mills;
    bool showLogo = false;

    unsigned long last_task_mills;
    unsigned long diff_mills;
    OledDisplay display;
    SignalLed receiverLed;
    SignalLed blueToothLed;
    DynamicScreen topBar;
    Screen topScreen;
    Screen bottomScreen;
    DynamicScreen statusBar;
    BuzzerTone buzzer;
    LoraRadioClass loRaRadio;
    KeyboardMorseCodeDecoder morseCode;
    Keypad keypad;
    int enterMenuCounter = 0;
    int keyInterval = 150;

    bool isDown = true;
    std::string message;
    char lastChar = '^';
    int samplePeriod;

    int userIndex = 0;
    Preferences preferences;
    std::map<std::string, UserInfo> users;
    std::list<int> topBarPattern = std::list<int>{9500, 500};
    std::list<int> statusBarPattern = std::list<int>{300, 200, 300, 200, 500, 10};

    OptionMenu optionMenu;
    int lastBatteryLevel = -1;


    bool initialGreeting = false;
    unsigned long start_mills = 0;

public:
    explicit MorseWalkieTalkie() : receiverLed(SignalLed(RECEIVER_LED, startLow)),
                                   blueToothLed(SignalLed(BLUETOOTH_LED, startLow)),
                                   topBar(DynamicScreen(&display, 0, 1, true)),
                                   topScreen(Screen(&display, 1, 5, false, false, 0)),
                                   bottomScreen(Screen(&display, 5, 7, true, true)),
                                   statusBar(DynamicScreen(&display, 7, 8, false)) {}

    void setup();

    void loop();

    void onEmit(char character, std::string raw) override;

    void onCharStart() override;

    void onCharEnd(char dotOrDash, int length) override;

    void onMainPressed() override;

    void onMainReleased() override;

    void onLeftPressed() override;

    void onLeftReleased() override;

    void onRightPressed() override;

    void onRightReleased() override;

    void onOkPressed() override;

    void onOkReleased() override;

    void onMessageReceived(LoraMessage message) override;

    void drawExtra() override;

private:
    void sendMessage(char character);

    void deleteLastKey();

    void readConfiguration();

    void drawBatterLevel();

    void saveConfiguration();

    UserInfo addUser(std::string chipId, std::string deviceName);

    UserInfo getUser(std::string chipId);

    void updateUserCounter(std::string chipId, int counter);

    bool isValidDevice();

};

#endif //ESP32MORSE_MORSE_WALKIE_TALKIE_HPP
