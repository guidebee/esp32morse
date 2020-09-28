//
// Created by Jing SHEN on 28/9/20.
//
#include <string>

struct Configuration {
    std::string channelId;
    std::string chipId;
    std::string deviceName;
    byte syncWord;
    bool playSound;
    bool keyFastSpeed; //true or false
    std::string loraFrequency;
    char encryptionKey[16];

};

extern Configuration globalConfiguration;