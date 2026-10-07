#include <Arduino.h>
#include <Wire.h>

#define I2C_SLAVE_ADDR 0x08
#define SDA_PIN 21
#define SCL_PIN 22

volatile uint8_t ultimo_dado = 0;

// Callback disparado quando o Master envia dados
void receiveEvent(int bytes) {
  while (Wire.available()) {
    ultimo_dado = Wire.read();
  }
  Serial.printf("[SLAVE] Dado recebido: %d\n", ultimo_dado);
}

// Callback disparado quando o Master requisita dados
void requestEvent() {
  // Retorna, por exemplo, o dobro do valor recebido
  uint8_t retorno = ultimo_dado * 3;
  Wire.write(retorno);
}

void setup() {
  Serial.begin(9600);
  
  // No ESP32 Arduino Core, passa os pinos e o endereço para iniciar como Slave
  Wire.begin(I2C_SLAVE_ADDR);
  
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.println("[SLAVE] Aguardando comunicacao...");
}

void loop() {
  delay(100);
}