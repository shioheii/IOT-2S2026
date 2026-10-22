// Bibliotecas Google Sheets
#include <WiFi.h>
#include <ESP_Google_Sheet_Client.h>

// Bibliotecas BLE
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEClient.h>
#include <BLERemoteCharacteristic.h>
#include <cstring>

// Identificadores BLE
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define PROJECT_ID "rich-sprite-510322-p8"

#define CLIENT_EMAIL \
"esp-signer-sa@rich-sprite-510322-p8.iam.gserviceaccount.com"

const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDUBjCCGKRPGAzN\n8cO76GpldmArKRmcXni+lL0d76c+ybieg/hWDkrDoxjhKg1n2gJSKpEW1L/0ASQG\nZWX3B1urU334PAzy5sBsRZclPCIGQfEIuf/e3uhxu2O32eTCXnWXbbuVaQIzHJiV\nbhwEelU+fn/VM/daxvIRXioUjO3QJFdgstexkS/tNGFL3hb6TTmpp9hnQ0D3/l3x\njMojRgL1AI7pdgF4Nm4Pv0xTsB+CmkI3XNBrq7+n19PMnaZWNHTmX2nMumfFeMFx\nFGr6nzY+5sRsdC2w2lKfJzDJFT2BKwpGtQ2hTr/hqgma6gXCUtvDYzL0dApwKgcG\nOf3MMqb1AgMBAAECggEAD1V+Qx5snbLH1EoopYmmzj08McroaQR1F5Vywvw+3H3i\nzgYN2w1wD0uXpEsyCoF8GvysnxnWYxuFAzhooi/ES7T/XolbR2L0ZjA3QPBjJpA0\n0jR/gpsc9QLAML7/uiXyO7gqMaFVUSR/jYkX2iQNz7X+4Ui/mi9auPJWOUPC/ENO\n37fHUhff7oFMPPsdHXbArEgZthgpCusxkChtcOaMuLM4/6Cf7KlnaFYCpMMwlj49\nO3Au9SKU8CDhmYhVn5i+ofsscZL75k84/Y9+4ASzyxyseVGa+NTX6ArddAxPqF16\nbfBYJkkj1cAxAEuujEbsz8hC4ipLgthq4mDAUL2uaQKBgQDu9hVSfk05DnvfoiNv\nyed1uUyO+67D5igZOi98Q1LyMQSDjV9nhmLIous+IPN2tXh6XzNEbL2R8Z+6lgqU\n0Iy7hWzcCAshmMDCdNaXkzsvMR+z8N17Z8yWZXohYZYycXBUTlxR1euz/HIyQ+Dq\nX93vQUWAtH81zrybR8ZvROn+CQKBgQDjJGfpeqhpFVc7hx2qAceILG3ry24UQaor\nT92oHCVDK+6qdUOvOR4Ods+1anjbOXYDOF/cX9n85rewFbnFydn/CoNLamG3H5nT\n+h9QGgdlFYDDmR1zz6xagvRlU9JmBTr1TYVo3dtLy/B9/hUlfKdWruoX0nBYSfys\nZJwb2XLcjQKBgBuxYxkzT+m71vk8xhSPdoZ7Gfc8Da3gP2dlCdnBx5wPuDEysgrC\nVTJhRxflI7HvJ+4umDdmzrVaJiOufb1vSc/1j38UY43aMQSYG8JnKqW2cLEeydwi\nVGBdlEDIGWrxII50olhNjUpHiEhw+2DOCV9P1ikrQc7PjaYFNGbyupdZAoGBAMnr\nz0RLefMQuZ99me1L76kqdf0rtwvi4/fk49NSUf3IzD5USHs4d3O8QyGvKQkZp8Nt\nFiq+OvcL0zeB2MfvjQ+gtN8SDxPPz9wVekinGvPNjc5UC06syzcbO0/omB2BgMwY\nwgRbVVuoTH50pS/SBKCWlhvfQ2f5PdmzBuPrBqTxAoGAN/pADVwWpaN8yyWcF0o8\nrnT93qWgMbN2BtyHSXmv0uBG5G4IB+eb+72Bq2WruIHumc3Paf09DziyIai4CPDv\nM6PTLv1F6TEe5OF/t2VM3tq/w0ngd/gPomfuHHJ50WIKpVK0xaXnVxBEt2lsPPAx\njPhEsXIuuEuf7Fm20VpQDrE=\n-----END PRIVATE KEY-----\n";

// ID da sua planilha Google Sheets
#define SPREADSHEET_ID "1c08SXuW4h0amf_1ew4WJW62ovUtIl2C7F6VsUH7wcbI"

// Nome da aba e célula inicial
#define SHEET_RANGE "Dados!A1"

#define WIFI_SSID "VaiTomando"
#define WIFI_PASSWORD "passa4e2carros"

#define LDR 34

// Objetos BLE
BLEScan *pBLEScan = nullptr;
BLEClient *pClient = nullptr;

BLERemoteCharacteristic *pRemoteCharacteristic = nullptr;

// Estado da conexão
bool connected = false;

unsigned long ultimoEnvio = 0;

const unsigned long intervalo = 1000;

String getHorario()
{
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo))
  {
    return "ERRO";
  }

  char horario[20];

  strftime(
    horario,
    sizeof(horario),
    "%H:%M:%S",
    &timeinfo
  );

  return String(horario);
}

void writeData(int16_t valorADS_1, int angulo_1, int valorADS_2, int angulo_2)
{
  if (!GSheet.ready())
  {
    Serial.println("Google Sheets ainda não está pronto.");
    return;
  }

  FirebaseJson response;
  FirebaseJson valueRange;

  String horario = getHorario();

  // Os dados serão enviados como UMA LINHA
  valueRange.add("majorDimension", "ROWS");

  valueRange.set("values/[0]/[0]", horario);
  valueRange.set("values/[0]/[1]", valorADS_1);
  valueRange.set("values/[0]/[2]", angulo_1);
  valueRange.set("values/[0]/[3]", valorADS_2);
  valueRange.set("values/[0]/[4]", angulo_2);

  Serial.println();
  Serial.println("Enviando para Google Sheets...");
  Serial.println("-----------------------------");

  bool success = GSheet.values.append(
    &response,
    SPREADSHEET_ID,
    SHEET_RANGE,
    &valueRange
  );

  if (success)
  {
    Serial.println("Dados enviados!");
    response.toString(Serial, true);
  }
  else
  {
    Serial.print("Erro: ");
    Serial.println(GSheet.errorReason());
  }
}

uint8_t valorRecebido;

// Callback de recebimento
void notifyCallback(
    BLERemoteCharacteristic *pCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify
) {

    Serial.println(length);
    Serial.println(sizeof(uint8_t));
    // Verifica se recebeu exatamente um float
    if (length == sizeof(uint8_t)) {


        memcpy(
            &valorRecebido,
            pData,
            sizeof(uint8_t)
        );

        Serial.print("Valor recebido: ");
        Serial.println(valorRecebido, 2);

        // Aqui você poderá atualizar
        // a variável utilizada pelo seu sistema.
    }
    else {

        Serial.println("Tamanho de pacote inesperado!");

    }
}

// Procura e conecta ao Server
bool conectarServer() {

    Serial.println("Procurando ESP32_SENSOR...");

    // Realiza uma busca de 5 segundos
    BLEScanResults *foundDevices =
        pBLEScan->start(5, false);

    for (int i = 0; i < foundDevices->getCount(); i++) {

        BLEAdvertisedDevice device =
            foundDevices->getDevice(i);

        // Verifica se o dispositivo anuncia nosso serviço
        if (device.isAdvertisingService(
            BLEUUID(SERVICE_UUID)
        )) {

            Serial.println("Servidor encontrado!");

            // Cria o Client
            pClient = BLEDevice::createClient();

            // Conecta ao Server
            if (!pClient->connect(device.getAddress())) {

                Serial.println("Falha na conexão!");
                pBLEScan->clearResults();

                return false;
            }

            Serial.println("Conectado!");

            // Localiza o serviço remoto
            BLERemoteService *pRemoteService =
                pClient->getService(
                    BLEUUID(SERVICE_UUID)
                );

            if (pRemoteService == nullptr) {

                Serial.println("Serviço não encontrado!");
                pClient->disconnect();
                pBLEScan->clearResults();

                return false;
            }

            // Localiza a característica remota
            pRemoteCharacteristic =
                pRemoteService->getCharacteristic(
                    BLEUUID(CHARACTERISTIC_UUID)
                );

            if (pRemoteCharacteristic == nullptr) {

                Serial.println("Característica não encontrada!");
                pClient->disconnect();
                pBLEScan->clearResults();

                return false;
            }

            // Habilita notificações
            if (pRemoteCharacteristic->canNotify()) {

                pRemoteCharacteristic->registerForNotify(
                    notifyCallback
                );

                Serial.println("Notificações habilitadas!");

            }
            else {

                Serial.println("Notify não disponível!");
                pClient->disconnect();
                pBLEScan->clearResults();

                return false;
            }

            connected = true;

            pBLEScan->clearResults();

            return true;
        }
    }

    Serial.println("Servidor não encontrado!");

    pBLEScan->clearResults();

    return false;
}

void setup() {

  Serial.begin(115200);

  // Inicializa BLE
  BLEDevice::init("ESP32_RECEIVER");

  // Configura o Scanner
  pBLEScan = BLEDevice::getScan();

  pBLEScan->setActiveScan(true);

  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  Serial.println("BLE Client iniciado!");


  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Conectando ao Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.println("Wi-Fi conectado!");

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // UTC-3 (horário de Brasília)
  configTime(
    -3 * 3600,
    0,
    "pool.ntp.org",
    "time.nist.gov"
  );

  Serial.println("Sincronizando horário...");

  struct tm timeinfo;

  while (!getLocalTime(&timeinfo))
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Horário: ");
  Serial.println(getHorario());

  Serial.println("Inicializando Google Sheets...");

  GSheet.begin(
    CLIENT_EMAIL,
    PROJECT_ID,
    PRIVATE_KEY
  );

  Serial.println("Google Sheets inicializado!");

  pinMode(LDR, INPUT);

}

void loop() {

    // Se não estiver conectado, tenta conectar
    if (!connected) {

        conectarServer();

        delay(1000);

    }

  // Executa a cada 1 segundo
  if (millis() - ultimoEnvio >= intervalo)
  {
    ultimoEnvio = millis();

    uint8_t adc0;
    int ldrIn;

    adc0 = valorRecebido;
    ldrIn = analogRead(LDR);

    int angulo_ADC = map(
      adc0,
      0,
      1100,
      0,
      180
    );

    int angulo_LDR = map(
      ldrIn,
      0,
      4095,
      0,
      180
    );
  
    angulo_LDR = constrain(
      angulo_LDR,
      0,
      180
    );

    // Limita entre 0 e 180
    angulo_ADC = constrain(
      angulo_ADC,
      0,
      180
    );

    Serial.println();
    Serial.println("============================");

    Serial.print("Horario: ");
    Serial.println(getHorario());

    Serial.print("ADC: ");
    Serial.println(adc0);

    Serial.print("Angulo ADC: ");
    Serial.println(angulo_ADC);

    Serial.print("LDR: ");
    Serial.println(ldrIn);

    Serial.print("Angulo LDR: ");
    Serial.println(angulo_LDR);

    writeData(
      adc0,
      angulo_ADC,
      ldrIn,
      angulo_LDR
    );

  }

  GSheet.ready();
}
