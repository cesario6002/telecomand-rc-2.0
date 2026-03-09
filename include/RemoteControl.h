#pragma once

#include <Arduino.h>
#include <RF24.h>

// Structure du paquet radio (max 32 octets)
struct Commande {
    int16_t gaz, gouv, prof, ail, volet;
    bool turbo, largueur, assist, takeoff, landing;
};

class RemoteControl {
public:
    RemoteControl(uint8_t ce, uint8_t csn);

    bool begin();
    void readInputs();
    bool sendData(Commande* cmd);
    Commande getPacket() { return _data; }

private:
    RF24 _radio;
    Commande _data;
};
