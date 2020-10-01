//
// Created by Jing SHEN on 27/9/20.
//
#include <string>
#include "morse_walkie_talkie.hpp"

#include "configuration.hpp"

#define MAX_MESSAGE_LENGTH 36
bool isOptionMode = false;
Configuration globalConfiguration;

void MorseWalkieTalkie::sendMessage(char character) {
    if (!message.empty()) {
        lastChar = ' ';
        loRaRadio.sendMessage(message);
        bottomScreen.print(character);
        auto morseText = morseCode.generateDitDashString(message);
        Serial.print(morseText.c_str());
        if (!muteSound) {
            buzzer.playMessageSent();
        }
        receiverLed.signalMessageSent();
        message = "";
    }
}


void MorseWalkieTalkie::deleteLastKey() {
    bottomScreen.backspace();
    message = message.substr(0, message.length() - 1);
}


void MorseWalkieTalkie::onEmit(char character, std::string raw) {

    switch (character) {
        case '*':
            deleteLastKey();
            break;
        case '^':
            if (!muteSound) {
                buzzer.playError();
            }
            receiverLed.signalError();
            break;
        default:
            if (!(character == ' ' and lastChar == ' ')) {
                if (character != '\n') {
                    message += character;
                    bottomScreen.print(character);
                    receiverLed.signalMorse(raw);
                }
            }
            lastChar = character;
            if (character == '\n') {
                sendMessage(character);
            }
            if (message.length() >= MAX_MESSAGE_LENGTH) {
                sendMessage('-');
            }
            break;
    }

}

void MorseWalkieTalkie::onCharStart() {

}

void MorseWalkieTalkie::onCharEnd(char dotOrDash, int length) {
    if (!muteSound) {
        if (dotOrDash == '.') {
            buzzer.playDi();
        } else if (dotOrDash == '-') {
            buzzer.playDah();
        }
    }
}


void MorseWalkieTalkie::onMainPressed() {
    last_mills = millis();

    isDown = true;

    morseCode.setOnOff(true);
}


void MorseWalkieTalkie::saveConfiguration() {
    preferences.begin("guidebeeit", false);
    preferences.putString("deviceName", "Harry");
    preferences.putString("channelId", "<1234>");
    preferences.putBool("keyFastSpeed", false);
    preferences.putUChar("syncWord", 0x34);
    preferences.putBool("playSound", true);
    preferences.end();
}


void MorseWalkieTalkie::readConfiguration() {
    uint64_t chipId = ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
    char buffer[64];
    sprintf(buffer, "%04X", (uint16_t) (chipId >> 32));//print High 2 bytes
    globalConfiguration.chipId = buffer;
    sprintf(buffer, "%08X", (uint32_t) chipId);//print Low 4bytes.
    globalConfiguration.chipId += buffer;
    //saveConfiguration();
    preferences.begin("guidebeeit", false);
    globalConfiguration.deviceName = preferences.getString("deviceName", "Morse").c_str();
    globalConfiguration.playSound = preferences.getBool("playSound", true);
    globalConfiguration.keyFastSpeed = preferences.getBool("keyFastSpeed", false);
    globalConfiguration.channelId = preferences.getString("channelId", "<1234>").c_str();
    globalConfiguration.syncWord = preferences.getUChar("syncWord", 0x34);
    preferences.end();
    Serial.printf("%s\n", globalConfiguration.deviceName.c_str());

    sprintf(globalConfiguration.encryptionKey, "guidebeeit202010");
    printf("Chip Id=%s\n", globalConfiguration.chipId.c_str());
}

void MorseWalkieTalkie::setup() {
// write your initialization code here

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

    while (!Serial && !Serial.available()) {}
    randomSeed(analogRead(0));
    // Pass log level, whether to show log level, and print interface.
    // Available levels are:
    // LOG_LEVEL_SILENT, LOG_LEVEL_FATAL, LOG_LEVEL_ERROR, LOG_LEVEL_WARNING, LOG_LEVEL_NOTICE, LOG_LEVEL_TRACE, LOG_LEVEL_VERBOSE
    // Note: if you want to fully remove all logging code, uncomment #define DISABLE_LOGGING in Logging.h
    //       this will significantly reduce your project size

    Log.begin(LOG_LEVEL_VERBOSE, &Serial);

    display.setup();
    topScreen.clearScreen();
    bottomScreen.clearScreen();
    topBar.clearScreen();
    statusBar.clearScreen();
    buzzer.setup();
    receiverLed.setup();
    blueToothLed.setup();

    loRaRadio.setup();
    Log.notice("LoRaRadio Initializing OK!");

    std::string app_name = "hi";
    topBar.displayText("Morse Walkie Talkie", topBarPattern);
    statusBar.displayText("     Guidebee IT", statusBarPattern, false);
    auto morseText = morseCode.generateDitDashString(app_name);
    buzzer.playMorseText(morseText);
    receiverLed.signalMorseText(morseText);
    blueToothLed.signalMorseText(morseText);

    keypad.setup();
    morseCode.addListener(this);
    keypad.addListener(this);
    loRaRadio.addListener(this);
    char letter = morseCode.getMorseLetter("...");

    Serial.println(letter);
    readConfiguration();
    last_mills = millis();
    current_mills = last_mills;
    last_task_mills = last_mills;
    diff_mills = 0;
    samplePeriod = morseCode.getSamplePeriod();
    optionMenu.setup();
}

void MorseWalkieTalkie::onMessageReceived(LoraMessage message) {
    switch (message.messageType) {
        case MESSAGE_TYPE_HELLO: {
            auto userInfo = addUser(message.chipId, message.payload);
            char buffer[64];
            int relativeRssi = (int) ((message.rssi + 200) / 40);
            sprintf(buffer, "%d:%s %.*s", userInfo.index, userInfo.deviceName.c_str(), relativeRssi, "-----");
            std::string deviceIndex = buffer;
            statusBar.displayText(deviceIndex,
                                  statusBarPattern, false);
            receiverLed.signalMessageReceived();
        }
            break;
        case MESSAGE_TYPE_TEXT_ACK: {
            auto userInfo = addUser(message.chipId, message.payload);
            char buffer[16];
            sprintf(buffer, "%d", userInfo.index);
            std::string deviceIndex = buffer;
            statusBar.displayText(deviceIndex + ":" + userInfo.deviceName + " OK",
                                  statusBarPattern, false);
            receiverLed.signalMessageReceived();
        }
            break;
        default: {
            auto userInfo = getUser(message.chipId);
            std::string deviceIndex = "";
            if (userInfo.index == 0) {
                userInfo = addUser(message.chipId, DEFAULT_DEVICE_NAME);
            }
            if (userInfo.index > 0) {
                char buffer[16];
                sprintf(buffer, "%d", userInfo.index);
                deviceIndex = buffer;
                statusBar.displayText(deviceIndex + ":" + userInfo.deviceName,
                                      statusBarPattern, false);
                userInfo.counter = message.counter;
            }
            String loRaData = (deviceIndex + ":" + message.payload).c_str();
            Serial.print(loRaData);
            topScreen.print(loRaData + '\n');
            buzzer.playMessageReceived();
            receiverLed.signalMessageReceived();
        }
            break;
    }

}


void MorseWalkieTalkie::loop() {
    if (!isOptionMode) {
        keypad.loop();
        buzzer.loop();
        receiverLed.loop();
        blueToothLed.loop();
        topBar.loop();
        statusBar.loop();

        current_mills = millis();
        unsigned long diff_task_mills = current_mills - last_task_mills;
        if (diff_task_mills >= samplePeriod) {
            if (!isDown) {
                morseCode.process(false);
            }
            last_task_mills = current_mills;
        }
        loRaRadio.loop();
    } else {
        optionMenu.loop();
    }


}

void MorseWalkieTalkie::onMainReleased() {
    isDown = false;

    diff_mills = millis() - last_mills;

    if (diff_mills <= KEY_INTERVAL) {

        morseCode.processKey(false);
    } else if (diff_mills < KEY_INTERVAL * 4) {

        morseCode.processKey(true);
    }

    morseCode.setOnOff(false);
}

void MorseWalkieTalkie::onLeftPressed() {


}


void MorseWalkieTalkie::onLeftReleased() {
    deleteLastKey();
}

void MorseWalkieTalkie::onRightPressed() {


}

void MorseWalkieTalkie::onRightReleased() {
    message += ' ';
    bottomScreen.print(' ');
}

void MorseWalkieTalkie::onOkPressed() {

}

void MorseWalkieTalkie::onOkReleased() {
    if (!message.empty()) {
        enterMenuCounter = 0;
        sendMessage('\n');
    } else {
        if (++enterMenuCounter >= ENTER_MENU_COUNT) {
            isOptionMode = true;
            enterMenuCounter = 0;
            optionMenu.drawOptionMenus();
        }

    }
}

UserInfo MorseWalkieTalkie::getUser(std::string chipId) {
    if (users.count(chipId) > 0) {
        return users[chipId];
    }
    return UserInfo();
}

UserInfo MorseWalkieTalkie::addUser(std::string chipId, std::string deviceName) {
    if (users.count(chipId) == 0) {
        UserInfo userInfo;
        userInfo.deviceName = deviceName;
        userInfo.counter = 0;
        userInfo.index = ++userIndex;
        users[chipId] = userInfo;
    } else {
        users[chipId].deviceName = deviceName;
    }
    return users[chipId];

}

void MorseWalkieTalkie::updateUserCounter(std::string chipId, int counter) {
    if (users.count(chipId) > 0) {
        users[chipId].counter = counter;
    }
}


