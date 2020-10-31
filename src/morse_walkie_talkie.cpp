//
// Created by Jing SHEN on 27/9/20.
//
#include <string>
#include "morse_walkie_talkie.hpp"
#include <soc/efuse_reg.h>
#include "configuration.hpp"

#define BATTERY_LEVEL_PIN 35
#define MAX_MESSAGE_LENGTH 36
#define POWER_SWITCH_PIN 33
#define BUTTON_PIN_BITMASK 0x200000000 // 2^33 in hex

bool isOptionMode = false;
Configuration globalConfiguration;
union {
    uint32_t parts[4];
    unsigned char bytes[16];
} encryptedChipId;

void MorseWalkieTalkie::sendMessage(char character) {
    if (!message.empty()) {
        lastChar = ' ';
        loRaRadio.sendMessage(message);
        bottomScreen.print(character);
        auto morseText = morseCode.generateDitDashString(message);
        Serial.print(morseText.c_str());
        if (globalConfiguration.playSound) {
            buzzer.playMessageSent();
        }
        receiverLed.signalMessageSent();
        message = "";
    }
}


void MorseWalkieTalkie::deleteLastKey() {
    bool deleted = bottomScreen.backspace();
    if (deleted) {
        message = message.substr(0, message.length() - 1);
    }
}


void MorseWalkieTalkie::onEmit(char character, std::string raw) {

    switch (character) {
        case '*':
            deleteLastKey();
            break;
        case '^':
            if (globalConfiguration.playSound) {
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
    if (globalConfiguration.playSound) {
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
    preferences.putBool("inputSpeed", false);
    preferences.putUChar("syncWord", 0x34);
    preferences.putBool("playSound", true);
    preferences.end();
}


void MorseWalkieTalkie::readConfiguration() {
    uint64_t chipId = ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
    char buffer[64];
    sprintf(buffer, "%08X", (uint16_t) (chipId >> 32));//print High 2 bytes
    globalConfiguration.chipId = buffer;
    sprintf(buffer, "%08X", (uint32_t) chipId);//print Low 4bytes.
    globalConfiguration.chipId += buffer;
    //saveConfiguration();
    preferences.begin("guidebeeit", false);
    globalConfiguration.deviceName = preferences.getString("deviceName", "Morse").c_str();
    globalConfiguration.playSound = preferences.getBool("playSound", true);
    globalConfiguration.inputSpeed = preferences.getBool("inputSpeed", false);
    globalConfiguration.bluetooth = preferences.getBool("bluetooth", false);
    globalConfiguration.channelIdValue = preferences.getInt("channelIdValue", 1234);
    globalConfiguration.channelId = preferences.getString("channelId", "<1234>").c_str();
    globalConfiguration.syncWord = preferences.getUChar("syncWord", 0x34);
    preferences.end();
    Serial.printf("%s\n", globalConfiguration.deviceName.c_str());
    if (globalConfiguration.inputSpeed) {
        keyInterval = 100;
    } else {
        keyInterval = 150;
    }
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
    pinMode(POWER_SWITCH_PIN, INPUT_PULLDOWN);
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    Serial.print("Enter deep sleep");

    if (supportSleep) {
        int value = digitalRead(POWER_SWITCH_PIN);
        if (value == HIGH) {
            // Serial.print("High\n");
        } else {
            // Serial.print("Low\n");
            esp_deep_sleep_start();
        }
    }
    readConfiguration();
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

    loRaRadio.setSyncWord(globalConfiguration.syncWord);
    last_mills = millis();
    current_mills = last_mills;
    last_task_mills = last_mills;
    diff_mills = 0;
    samplePeriod = morseCode.getSamplePeriod();
    pinMode(BATTERY_LEVEL_PIN, INPUT);
    optionMenu.setup();

    if (needCheckDevice) {
        bool isValid = isValidDevice();
        if (!isValid) {
            topScreen.print("Not a valid device\nReboot in 5s\n");
            statusBar.displayText("Copyright Guidebee IT\n", statusBarPattern, true);
            delay(5000);
            esp_deep_sleep_start();
        } else {
            Serial.printf("Device validation Ok\n");
        }
    }

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
            drawBatterLevel();
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
            drawBatterLevel();
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
                drawBatterLevel();
                userInfo.counter = message.counter;
            }
            String loRaData = (deviceIndex + ":" + message.payload).c_str();
            Serial.print(loRaData);
            topScreen.print(loRaData + '\n');
            if (globalConfiguration.playSound) {
                buzzer.playMessageReceived();
            }
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
            drawBatterLevel();
            last_task_mills = current_mills;
        }
        loRaRadio.loop();
    } else {

        optionMenu.loop();
    }
    if (supportSleep) {
        int value = digitalRead(POWER_SWITCH_PIN);
        if (value == HIGH) {
            // Serial.print("High\n");
        } else {
            // Serial.print("Low\n");
            esp_deep_sleep_start();
        }

    }
}

void MorseWalkieTalkie::onMainReleased() {
    isDown = false;

    diff_mills = millis() - last_mills;
    if (globalConfiguration.inputSpeed) {
        keyInterval = 100;
    } else {
        keyInterval = 150;
    }

    if (diff_mills <= keyInterval) {

        morseCode.processKey(false);
    } else if (diff_mills < keyInterval * 4) {

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
            buzzer.stop();
            buzzer.stop();
            enterMenuCounter = 0;
            optionMenu.drawClientArea();
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

void MorseWalkieTalkie::drawExtra() {
    drawBatterLevel();
}


void MorseWalkieTalkie::drawBatterLevel() {
    int batteryBarWidth = 10;
    float batteryLevel = map(analogRead(BATTERY_LEVEL_PIN), 0.0f, 4095.0f, 0, 100);
    int barWidth = (int) (batteryLevel * batteryBarWidth / 100.0);
    char battery[64];
    sprintf(battery, "v=%02.2f volts", batteryLevel);


    //if (lastBatteryLevel != barWidth)
    {
        display.fillRect(128 - batteryBarWidth, 56, batteryBarWidth, 8, BLACK);
        display.drawRect(128 - batteryBarWidth, 57, batteryBarWidth, 6, WHITE);
        display.fillRect(128 - batteryBarWidth, 57, barWidth, 6, WHITE);
        lastBatteryLevel = barWidth;
        //loRaRadio.sendMessage(battery);
        sprintf(battery, "v=%02.2f volts\n", batteryLevel);
        //topScreen.print(battery);

        Serial.printf(battery);
    }

}


bool MorseWalkieTalkie::isValidDevice() {

    unsigned char buffer[64];
    ::encrypt(const_cast<char *>(globalConfiguration.chipId.c_str()), globalConfiguration.encryptionKey, buffer);
    int count = 0;
    encryptedChipId.parts[0] = REG_GET_FIELD(EFUSE_BLK3_RDATA4_REG, EFUSE_BLK3_DOUT4);
    encryptedChipId.parts[1] = REG_GET_FIELD(EFUSE_BLK3_RDATA5_REG, EFUSE_BLK3_DOUT5);
    encryptedChipId.parts[2] = REG_GET_FIELD(EFUSE_BLK3_RDATA6_REG, EFUSE_BLK3_DOUT6);
    encryptedChipId.parts[3] = REG_GET_FIELD(EFUSE_BLK3_RDATA7_REG, EFUSE_BLK3_DOUT7);
    for (int i = 0; i < 16; i++) {
        Serial.printf("%02x", encryptedChipId.bytes[i]);
        if (buffer[i] == encryptedChipId.bytes[i]) {
            count++;
        }
    }
    Serial.printf("\n");
    return count > 8;
}

