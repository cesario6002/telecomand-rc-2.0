// le code  de la telecommande 

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

int verifie;
RF24 radio(21, 5);  // CE, CSN

      //joystick 1
        //int x1 = 33;
        //int y1 = 35;
      //joystick 2
        //int x2 = 12;
        //int y2 = 13;
      //joystick 3
        //int x3 = 14; volet
      //Switches
        //int sw1 = 32; turbo
        //int sw2 = 34; largeur

//initialisation des variables paquet a envoyer

struct Commande
{
  int gaz;
  int ailleron;
  int prof;
  int gouv;
  int volet;
  int turbo;
  int largueur;
};
Commande envoi;
void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.openWritingPipe(0xF0F0F0F0E1LL);  // Adresse du récepteur
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  Serial.println("Émetteur prêt !");
}

void loop() {
  
  //lecture des valeurs des joysticks
  envoi.gaz = map(analogRead(35), 0, 4095, 1000, 2000); //joystick 1
  envoi.ailleron = map(analogRead(33), 0, 4095, 45, 135); //joystick 2
  envoi.prof = map(analogRead(13), 0, 4095, 0, 130);
  envoi.gouv = map(analogRead(12), 0, 4095, 120, 30); //joystick 2
  envoi.volet = 0; //joystick 3
  envoi.turbo = digitalRead(32); //switch 1
  envoi.largueur = digitalRead(34); //switch 2
  if (digitalRead(14)==1){
    envoi.volet = 30 ; 
  }  
  radio.write(&envoi, sizeof(envoi));
  verifie = radio.write(&envoi, sizeof(envoi));
  if (verifie = 0) {
    
    Serial.println("Erreur d'envoi !");
  }
  delay(10);  // Attendre 10 ms avant le prochain envoi
}
