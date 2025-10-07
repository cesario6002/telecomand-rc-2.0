// le code  de la telecommande 

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

int verifie;
RF24 radio(21, 5);  // CE, CSN

      //joystick 1
        //int x1 = 34;
        //int y1 = 35;
      //joystick 2
        //int x2 = 12;
        //int y2 = 13;

//initialisation des variables paquet a envoyer

struct Commande
{
  int gaz;
  int ailleron;
  int prof;
  int gouv;
};
Commande envoi;
void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.openWritingPipe(0xF0F0F0F0E1LL);  // Adresse du récepteur
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_1MBPS);
  Serial.println("Émetteur prêt !");
}

void loop() {
  
  //lecture des valeurs des joysticks
  envoi.gaz = map(analogRead(35), 0, 4095, 0, 225); //joystick 1
  envoi.ailleron = map(analogRead(12), 0, 4095, 45, 135); //joystick 2
  envoi.prof = map(analogRead(13), 0, 4095, 0, 180); //joystick 2
  envoi.gouv = map(analogRead(33), 0, 4095, 0, 180); //joystick 2
  delay(10);
  // Message à envoyer
  radio.write(&envoi, sizeof(envoi));
  verifie = radio.write(&envoi, sizeof(envoi));

  if (!verifie) {
    Serial.println("Erreur d'envoi !");
  }
  delay(10);  // Attendre 50 ms avant le prochain envoi
}
