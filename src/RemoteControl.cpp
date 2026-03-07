#include "RemoteControl.h"

RemoteControl::RemoteControl(uint8_t ce, uint8_t csn, uint8_t pGaz, 
                             uint8_t pGouv, uint8_t pProf, uint8_t pAil, uint8_t pVolet,
                             uint8_t pTurbo, uint8_t pLargueur,
                             uint8_t pAssist, uint8_t pTakeoff, uint8_t pLanding)
    : _radio(ce, csn), 
      _pinGaz(pGaz), _pinGouv(pGouv), _pinProf(pProf), _pinAil(pAil), _pinVolet(pVolet),
      _pinTurbo(pTurbo), _pinLargueur(pLargueur), 
      _pinAssist(pAssist), _pinTakeoff(pTakeoff), _pinLanding(pLanding) {}

bool RemoteControl::begin() {
    pinMode(_pinTurbo, INPUT);
    pinMode(_pinLargueur, INPUT);
    pinMode(_pinAssist, INPUT);
    pinMode(_pinTakeoff, INPUT);
    pinMode(_pinLanding, INPUT);
    _radio.begin();
    if (!_radio.begin()) return false;
    _radio.openWritingPipe(0xF0F0F0F0E1LL);
    _radio.setPALevel(RF24_PA_HIGH);
    _radio.setDataRate(RF24_250KBPS); // Portée max [8]
    _radio.setChannel(76); // Canal moins encombré
    _radio.stopListening();
    return true;
}

void RemoteControl::readInputs() {
    // Joysticks sur ADC1 (Stable avec Wi-Fi) [9]
    _data.gaz      = map(analogRead(_pinGaz), 0, 4095, 1000, 2000);
    _data.gouv     = map(analogRead(_pinGouv), 0, 4095, 1000, 2000);
    _data.prof     = map(analogRead(_pinProf), 0, 4095, 1000, 2000);
    _data.ail      = map(analogRead(_pinAil), 0, 4095, 1000, 2000);
    _data.volet    = map(analogRead(_pinVolet), 0, 4095, 0, 30); // Pont diviseur

    // Boutons et Sécurité (Input-only pins)
    _data.turbo    = digitalRead(_pinTurbo); 
    _data.largueur = digitalRead( _pinLargueur);
    _data.assist   = digitalRead( _pinAssist);
    _data.takeoff  = digitalRead( _pinTakeoff);
    _data.landing  = digitalRead( _pinLanding);

    if (_data.assist) _data.volet = 30; // Priorité logicielle
}

bool RemoteControl::sendData() {
    return _radio.write(&_data, sizeof(_data));
}