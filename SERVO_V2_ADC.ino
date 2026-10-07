#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
// Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Inicializando ADS");

  if (!ads.begin()) {
    Serial.println("Falha ao inicializar o ADS.");
    while (1);
  }
}

void loop(void)
{
  int16_t adc0;
  int angulo;

  adc0 = ads.readADC_SingleEnded(0);
  angulo = map(adc0, 3000, 17600, 0, 180);

  Serial.printf("[MASTER] - Leitura ADC: %d | Angulo SERVO: %d\n", adc0, angulo);
  delay(500);
}