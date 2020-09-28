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

struct LoraMessage {
    //header: 48 bytes
    std::string channelId; //6 bytes
    std::string chipId;//6 bytes
    std::string repeaterId;//6 bytes
    int counter;//6 bytes
    std::string messageType;//2 bytes
    int length;//2 bytes
    std::string reserved1;//4 bytes
    std::string reserved2;//8 bytes
    std::string reserved3;//8 bytes
    std::string payload;//max 80 bytes

    bool valid; //invalid message

};


class LoraMessageListener {
public:
    virtual void onMessageReceived(LoraMessage message) = 0;
};

class LoraRadioClass : public LoRaClass {

private:
    byte _syncWord;
    LoraMessage _loraMessage;
public:

    explicit LoraRadioClass(byte syncWord = 0x34) : _syncWord(syncWord) {

    }

    void setup();

    void loop();

    void sendMessage(std::string message);

    void addListener(LoraMessageListener *listener);

private:
    LoraMessageListener *loraMessageListener = nullptr;

};


#endif //ESP32MORSE_LORA_RADIO_HPP
