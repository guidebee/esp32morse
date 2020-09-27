//
// Created by Jing SHEN on 27/9/20.
//

#include "morse_walkie_talkie.hpp"

void MorseWalkieTalkie::sendMessage(char character) {
    if (message != "") {
        lastChar = ' ';
        LoRaRadio.beginPacket();
        LoRaRadio.print(message.c_str());
        LoRaRadio.endPacket();
        bottomScreen.print(character);
        auto morseText = morseCode.generateDitDashString(message);
        Serial.print(morseText.c_str());
        buzzer.playMessageSent();
        receiverLed.signalMessageSent();

        message = "";
    }
}


void MorseWalkieTalkie::deleteLastKey() {
    bottomScreen.backspace();
    message = message.substr(0, message.length() - 1);
}


void MorseWalkieTalkie::onEmit(char character, std::string raw) {
    if (character == '*') {
        deleteLastKey();
    } else {
        //Send LoRaRadio packet to receiver
        if (!(character == ' ' and lastChar == ' ')) {
            if (character != '\n') {
                message += character;
                bottomScreen.print(character);
                //Serial.print(raw.c_str());
                //buzzer.playMorse(raw);
                receiverLed.signalMorse(raw);

            }
        }
        lastChar = character;
        if (character == '\n') {
            sendMessage(character);
            // topScreen.backspace();


        }
    }
}

void MorseWalkieTalkie::onCharStart() {

}

void MorseWalkieTalkie::onCharEnd(char dotOrDash, int length) {
    if (dotOrDash == '.') {
        buzzer.playDi();
    } else if (dotOrDash == '-') {
        buzzer.playDah();
    }
}


void MorseWalkieTalkie::onMainPressed() {
    last_mills = millis();

    isDown = true;

    morseCode.setOnOff(true);
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

    LoRaRadio.setup();
    Log.notice("LoRaRadio Initializing OK!");

    std::string app_name = "sos";
    topBar.print("morse walkie talkie");
    auto morseText = morseCode.generateDitDashString(app_name);
    buzzer.playMorseText(morseText);
    receiverLed.signalMorseText(morseText);
    blueToothLed.signalMorseText(morseText);

    keypad.setup();
    morseCode.addListener(this);
    keypad.addListener(this);
    char letter = morseCode.getMorseLetter("...");

    Serial.println(letter);
    chipId = ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
    printf("ESP32 Chip ID = %04X", (uint16_t) (chipId >> 32));//print High 2 bytes
    printf("%08X\n", (uint32_t) chipId);//print Low 4bytes.
    last_mills = millis();
    current_mills = last_mills;
    last_task_mills = last_mills;
    diff_mills = 0;
    samplePeriod = morseCode.getSamplePeriod();


}

void MorseWalkieTalkie::loop() {
// write your code here
    keypad.loop();
    buzzer.loop();
    receiverLed.loop();
    blueToothLed.loop();
    current_mills = millis();
    unsigned long diff_task_mills = current_mills - last_task_mills;
    if (diff_task_mills >= samplePeriod) {
        if (!isDown) {
            morseCode.process(false);
        }

        last_task_mills = current_mills;
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
        topScreen.print(LoRaData + '\n');

        buzzer.playMessageReceived();
        receiverLed.signalMessageReceived();


    }

}

void MorseWalkieTalkie::onMainReleased() {
    isDown = false;

    diff_mills = millis() - last_mills;

    if (diff_mills < KEY_INTERVAL) {

        morseCode.processKey(false);
    } else if (diff_mills < KEY_INTERVAL * 4) {

        morseCode.processKey(true);
    }

    morseCode.setOnOff(false);
}

void MorseWalkieTalkie::onLeftPressed() {
    deleteLastKey();

}


void MorseWalkieTalkie::onLeftReleased() {

}

void MorseWalkieTalkie::onRightPressed() {
    receiverLed.signalMessageSent();

}

void MorseWalkieTalkie::onRightReleased() {

}

void MorseWalkieTalkie::onOkPressed() {
    sendMessage('\n');
}

void MorseWalkieTalkie::onOkReleased() {

}
