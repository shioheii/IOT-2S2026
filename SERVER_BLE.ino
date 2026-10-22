#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Identificadores BLE
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Objetos BLE
BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;

// Controle de tempo
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

// Função de leitura do sensor
uint8_t lerSensor() {
    // Substituir pela leitura real do sensor
    return 200.0;
}

void setup() {

    Serial.begin(115200);

    // Inicializa o BLE
    BLEDevice::init("ESP32_SENSOR");

    // Cria o servidor
    pServer = BLEDevice::createServer();

    // Cria o serviço
    BLEService *pService =
        pServer->createService(SERVICE_UUID);

    // Cria a característica
    pCharacteristic =
        pService->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
        );

    // Habilita notificações
    pCharacteristic->addDescriptor(new BLE2902());

    // Valor inicial
    uint8_t valorInicial = 0.0;

    pCharacteristic->setValue(
        (uint8_t*)&valorInicial,
        sizeof(valorInicial)
    );

    // Inicia o serviço
    pService->start();

    // Configura Advertising
    BLEAdvertising *pAdvertising =
        BLEDevice::getAdvertising();

    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);

    // Inicia o anúncio
    BLEDevice::startAdvertising();

    Serial.println("BLE Server iniciado!");
}

void loop() {

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {

        previousMillis = currentMillis;

        // Obtém o valor do sensor
        uint8_t valorSensor = lerSensor();

        // Atualiza a característica BLE
        pCharacteristic->setValue(
            (uint8_t*)&valorSensor,
            sizeof(valorSensor)
        );

        // Envia notificação
        pCharacteristic->notify();

        Serial.print("Valor enviado: ");
        Serial.println(valorSensor);
    }
}
