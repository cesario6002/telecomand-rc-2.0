// le code  de la telecommande
#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "RemoteControl.h"
#include "Config.h"

RemoteControl remote(RADIO_CE, RADIO_CSN);

// File de message pour le transfert sécurisé entre les cœurs 1 et 0
QueueHandle_t packetQueue;

// Tâche de Capture (Cœur 1) : Priorité 5
void captureTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(20); // 50Hz
    Commande dataSnapshot; // Copie locale pour la fille de messages
    for(;;) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency); // Synchronisation à 50Hz
        remote.readInputs();
        dataSnapshot = remote.getPacket(); 
        xQueueOverwrite(packetQueue, &dataSnapshot);
    }
}

// Tâche Radio (Cœur 0) : Priorité 4
void radioTask(void *pvParameters) {
    Commande cmdToSend;
    for(;;) {

        if (xQueueReceive(packetQueue, &cmdToSend, portMAX_DELAY)) {

            if (!remote.sendData(&cmdToSend)) {
                Serial.println("Erreur de liaison radio");
            }
        }
    }
}

void setup() {
    Serial.begin(115200); 
    // Création de la file de données (boîte aux lettres sécurisée) [2]
    packetQueue = xQueueCreate(1, sizeof(Commande));

    // Initialisation matérielle
    if (!remote.begin()) {
        Serial.println("ERREUR : NRF24L01 introuvable !");
        while(1); // Arrêt de sécurité
    }

    // Lancement des tâches sur les deux cœurs de l'ESP32 [7, 8]
    // Capture sur Cœur 1 (appli) | Radio sur Cœur 0 (système/RF)
    xTaskCreatePinnedToCore(captureTask, "Capture", 4096, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(radioTask, "Radio", 4096, NULL, 4, NULL, 0);

    Serial.println(">> Télécommande prête : Vol 50Hz activé.");
}
void loop() {
    vTaskDelete(NULL); 
}