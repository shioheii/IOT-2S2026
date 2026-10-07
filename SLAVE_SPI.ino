#include <ESP32SPISlave.h>

#define PIN_MISO 19
#define PIN_MOSI 23
#define PIN_CLK  18
#define PIN_CS   5

ESP32SPISlave slave;

static constexpr size_t BUFFER_SIZE = 1;
uint8_t tx_buf[BUFFER_SIZE];
uint8_t rx_buf[BUFFER_SIZE];

void setup() {
  Serial.begin(9600);

  slave.setDataMode(SPI_MODE0);   // tem que bater com o mestre
  slave.begin(HSPI, PIN_CLK, PIN_MISO, PIN_MOSI, PIN_CS);

  Serial.println("SPI escravo inicializado...");
}

void loop() {
  tx_buf[0] = 0x12;   // byte que o escravo devolve pro mestre
  rx_buf[0] = 0x00;

  // bloqueia aqui até o mestre iniciar a transacao (puxar CS + clock)
  slave.transfer(tx_buf, rx_buf, BUFFER_SIZE);

  Serial.printf("Recebido do mestre: 0x%02X\n", rx_buf[0]);
}