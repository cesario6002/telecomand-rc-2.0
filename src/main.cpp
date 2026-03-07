// le code  de la telecommande
#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>


#include "RemoteControl.h"

// Instanciation de la télécommande avec tes broches spécifiques
// Ordre : CE, CSN, Gaz, Gouv, Prof, Ail, Volet, Turbo, Largueur, Assist, Takeoff, Landing
RemoteControl remote(21, 5, 36, 33, 39, 32, 35, 34, 27, 14, 26, 13);

const TickType_t xFrequency = pdMS_TO_TICKS(20); // Cadence de 50Hz (20ms) [3]

// Tâche de Capture (Cœur 1) : Priorité haute pour la réactivité des sticks
void captureTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency); // Intervalle stable pour le PID [3]
        remote.readInputs();
    }
}

// Tâche Radio (Cœur 0) : Gère l'envoi sans ralentir la lecture des capteurs
void radioTask(void *pvParameters) {
    for(;;) {
        if (!remote.sendData()) {
            Serial.println("Erreur de liaison radio"); // Debug via le moniteur [4]
        }
        vTaskDelay(xFrequency); // Envoi synchronisé à 50Hz
      }
}

void setup() {
    Serial.begin(115200); // Vitesse standard pour l'ESP32 [5]
    
    if (!remote.begin()) {
        Serial.println("Échec de l'initialisation matérielle !");
        while(1); // Arrêt de sécurité si le NRF24 est absent [6]
    }

    // Création des tâches FreeRTOS sur les deux cœurs [2, 7]
    xTaskCreatePinnedToCore(captureTask, "Capture", 4096, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(radioTask, "Radio", 4096, NULL, 4, NULL, 0);

    Serial.println("Système de vol prêt !");
}

void loop() {
    // La boucle loop est supprimée pour laisser FreeRTOS gérer les priorités [8]
    vTaskDelete(NULL); 
}
