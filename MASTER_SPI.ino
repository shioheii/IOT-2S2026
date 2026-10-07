#include <SPI.h>

#define PIN_MISO 19
#define PIN_MOSI 23
#define PIN_CLK  18
#define PIN_CS   5

void setup() {
  Serial.begin(9600);

  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH);

  SPI.begin(PIN_CLK, PIN_MISO, PIN_MOSI, PIN_CS);

  Serial.println("SPI mestre inicializado...");
}

void loop() {
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
  digitalWrite(PIN_CS, LOW);

  byte rx = SPI.transfer(0x34);   // byte que o mestre envia

  digitalWrite(PIN_CS, HIGH);
  SPI.endTransaction();

  Serial.printf("Escravo respondeu: 0x%02X\n", rx);

  delay(500);
}