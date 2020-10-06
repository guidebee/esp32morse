//
// Created by Jing SHEN on 19/9/20.
//

#ifndef ESP32MORSE_LORA_RADIO_HPP
#define ESP32MORSE_LORA_RADIO_HPP

#include <LoRa.h>
#include <string>

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
#define BAND 915E6

#define MESSAGE_TYPE_HELLO 0
#define MESSAGE_TYPE_TEXT 1
#define MESSAGE_TYPE_TEXT_ACK 2

#define DEFAULT_DEVICE_NAME "Morse User"
void encrypt(char *plainText, char *key, unsigned char *outputBuffer);
void decrypt(unsigned char *cipherText, char *key, unsigned char *outputBuffer);
struct LoraMessage {
    //header: 48 bytes
    std::string channelId; //6 bytes
    std::string chipId;//12 bytes
    std::string repeaterId;//12 bytes or other uses if not for repeater
    int counter;//5 bytes
    bool encrypted; //1 byte
    int messageType;//2 bytes
    int length;//2 bytes
    std::string reserved;//8 bytes
    std::string payload;//max 80 bytes
    bool valid; //invalid message
    int rssi;

};


class LoraMessageListener {
public:
    virtual void onMessageReceived(LoraMessage message) = 0;
};

class LoraRadioClass : public LoRaClass {

private:
    byte _syncWord;
    int _counter;
    LoraMessage _loraMessage;
    unsigned long _last_time;
public:

    explicit LoraRadioClass(byte syncWord = 0x34) : _syncWord(syncWord) {
        _last_time = 0;
    }

    void setup();

    void loop();

    void sendMessage(std::string message, int type = MESSAGE_TYPE_TEXT, bool encrypted = true);

    void addListener(LoraMessageListener *listener);



private:
    LoraMessageListener *loraMessageListener = nullptr;

    void sendHello();

    void sendAck();

    std::string encodeMessage(int type, std::string message, bool encrypted = true);

    void decodeMessage(String message);

    std::string encryptPayload(std::string payload);

    std::string decryptPayload(std::string payload);

};


#endif //ESP32MORSE_LORA_RADIO_HPP
