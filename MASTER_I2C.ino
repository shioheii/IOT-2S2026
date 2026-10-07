#include <Arduino.h>
#include <Wire.h>

#define I2C_SLAVE_ADDR 0x08
#define SDA_PIN 21
#define SCL_PIN 22

uint8_t contador = 45;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Inicializa I2C como Master nos pinos padrão
  Serial.println("[MASTER] Inicializado.");
}

void loop() {
  // 1. Envio de dados para o Slave
  Serial.printf("[MASTER] Enviando valor: %d\n", contador);
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write(contador);
  Wire.endTransmission();

  delay(1000);

  // 2. Requisição de dados do Slave (espera 1 byte de resposta)
  Wire.requestFrom(I2C_SLAVE_ADDR, 1);
  if (Wire.available()) {
    uint8_t resposta = Wire.read();
    Serial.printf("[MASTER] Resposta recebida do Slave: %d\n", resposta);
  } else {
    Serial.println("[MASTER] Sem resposta do Slave.");
  }

  //contador++;
  delay(1000);
}