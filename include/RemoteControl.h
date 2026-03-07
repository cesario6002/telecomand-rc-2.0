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
    RemoteControl(uint8_t ce, uint8_t csn, uint8_t pGaz, 
                  uint8_t pGouv, uint8_t pProf, uint8_t pAil, uint8_t pVolet,
                  uint8_t pTurbo, uint8_t pLargueur,
                  uint8_t pAssist, uint8_t pTakeoff, uint8_t pLanding);

    bool begin();
    void readInputs();
    bool sendData();
    Commande getPacket() { return _data; }

private:
    uint8_t _pinGaz, _pinGouv, _pinProf, _pinAil, _pinVolet, _pinTurbo, _pinLargueur, _pinAssist, _pinTakeoff, _pinLanding; // Stockage interne
    RF24 _radio;
    Commande _data;
};
