#include "RemoteControl.h"
#include "Config.h"
RemoteControl::RemoteControl(uint8_t ce, uint8_t csn)
    : _radio(ce, csn) {}

bool RemoteControl::begin() {
    pinMode(PIN_TURBO, INPUT);
    pinMode(PIN_LARGUEUR, INPUT);
    pinMode(PIN_ASSIST, INPUT);
    pinMode(PIN_TAKEOFF, INPUT);
    pinMode(PIN_LANDING, INPUT);
    // Initialize radio 
    if (!_radio.begin()) return false;
    _radio.openWritingPipe(0xF0F0F0F0E1LL);
    _radio.setPALevel(RF24_PA_HIGH);
    _radio.setDataRate(RF24_250KBPS); // Portée max [8]
    _radio.setChannel(76); // Canal moins encombré
    _radio.stopListening();
    return true;
}

void RemoteControl::readInputs() {
    // 1. Lecture et conversion avec vos plages spécifiques
    int bruteGaz  = map(analogRead(PIN_GAZ), 2448, 3142, 1000, 2000);
    int bruteProf = map(analogRead(PIN_PROF), 1186, 2286, 0, 180);
    int bruteAil  = 225- map(analogRead(PIN_AIL), 2690, 1430, 0, 180); 
    int bruteGouv = map(analogRead(PIN_GOUV), 0, 4095, 0, 180); 

    // 2. Application du lissage (Seuil de 2 degrés/unités)
    // On ne met à jour la valeur envoyée que si le mouvement est significatif
    if (abs(bruteGaz - _data.gaz) > 5)   _data.gaz  = bruteGaz;  // Gaz (plus souple)
    if (abs(bruteProf - _data.prof) > 5) _data.prof = bruteProf; // Profondeur
    if (abs(bruteAil - _data.ail) > 5)   _data.ail  = constrain(bruteAil, 50, 170);  // Ailerons
    if (abs(bruteGouv - _data.gouv) > 5) _data.gouv = bruteGouv; // Dérive

    // 3. Boutons et interrupteurs
    _data.assist   = !digitalRead(PIN_ASSIST);
    _data.turbo    = !digitalRead(PIN_TURBO);
    _data.largueur = !digitalRead(PIN_LARGUEUR);
}

bool RemoteControl::sendData(Commande* cmd) {
    return _radio.write(cmd, sizeof(Commande));
}