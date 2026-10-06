#include <Adafruit_ADS1X15.h>
#include <WiFi.h>
#include <ESP_Google_Sheet_Client.h>
#include <time.h>

// =====================================================
// GOOGLE CLOUD
// =====================================================

#define PROJECT_ID "rich-sprite-510322-p8"

#define CLIENT_EMAIL \
"esp-signer-sa@rich-sprite-510322-p8.iam.gserviceaccount.com"

const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDUBjCCGKRPGAzN\n8cO76GpldmArKRmcXni+lL0d76c+ybieg/hWDkrDoxjhKg1n2gJSKpEW1L/0ASQG\nZWX3B1urU334PAzy5sBsRZclPCIGQfEIuf/e3uhxu2O32eTCXnWXbbuVaQIzHJiV\nbhwEelU+fn/VM/daxvIRXioUjO3QJFdgstexkS/tNGFL3hb6TTmpp9hnQ0D3/l3x\njMojRgL1AI7pdgF4Nm4Pv0xTsB+CmkI3XNBrq7+n19PMnaZWNHTmX2nMumfFeMFx\nFGr6nzY+5sRsdC2w2lKfJzDJFT2BKwpGtQ2hTr/hqgma6gXCUtvDYzL0dApwKgcG\nOf3MMqb1AgMBAAECggEAD1V+Qx5snbLH1EoopYmmzj08McroaQR1F5Vywvw+3H3i\nzgYN2w1wD0uXpEsyCoF8GvysnxnWYxuFAzhooi/ES7T/XolbR2L0ZjA3QPBjJpA0\n0jR/gpsc9QLAML7/uiXyO7gqMaFVUSR/jYkX2iQNz7X+4Ui/mi9auPJWOUPC/ENO\n37fHUhff7oFMPPsdHXbArEgZthgpCusxkChtcOaMuLM4/6Cf7KlnaFYCpMMwlj49\nO3Au9SKU8CDhmYhVn5i+ofsscZL75k84/Y9+4ASzyxyseVGa+NTX6ArddAxPqF16\nbfBYJkkj1cAxAEuujEbsz8hC4ipLgthq4mDAUL2uaQKBgQDu9hVSfk05DnvfoiNv\nyed1uUyO+67D5igZOi98Q1LyMQSDjV9nhmLIous+IPN2tXh6XzNEbL2R8Z+6lgqU\n0Iy7hWzcCAshmMDCdNaXkzsvMR+z8N17Z8yWZXohYZYycXBUTlxR1euz/HIyQ+Dq\nX93vQUWAtH81zrybR8ZvROn+CQKBgQDjJGfpeqhpFVc7hx2qAceILG3ry24UQaor\nT92oHCVDK+6qdUOvOR4Ods+1anjbOXYDOF/cX9n85rewFbnFydn/CoNLamG3H5nT\n+h9QGgdlFYDDmR1zz6xagvRlU9JmBTr1TYVo3dtLy/B9/hUlfKdWruoX0nBYSfys\nZJwb2XLcjQKBgBuxYxkzT+m71vk8xhSPdoZ7Gfc8Da3gP2dlCdnBx5wPuDEysgrC\nVTJhRxflI7HvJ+4umDdmzrVaJiOufb1vSc/1j38UY43aMQSYG8JnKqW2cLEeydwi\nVGBdlEDIGWrxII50olhNjUpHiEhw+2DOCV9P1ikrQc7PjaYFNGbyupdZAoGBAMnr\nz0RLefMQuZ99me1L76kqdf0rtwvi4/fk49NSUf3IzD5USHs4d3O8QyGvKQkZp8Nt\nFiq+OvcL0zeB2MfvjQ+gtN8SDxPPz9wVekinGvPNjc5UC06syzcbO0/omB2BgMwY\nwgRbVVuoTH50pS/SBKCWlhvfQ2f5PdmzBuPrBqTxAoGAN/pADVwWpaN8yyWcF0o8\nrnT93qWgMbN2BtyHSXmv0uBG5G4IB+eb+72Bq2WruIHumc3Paf09DziyIai4CPDv\nM6PTLv1F6TEe5OF/t2VM3tq/w0ngd/gPomfuHHJ50WIKpVK0xaXnVxBEt2lsPPAx\njPhEsXIuuEuf7Fm20VpQDrE=\n-----END PRIVATE KEY-----\n";

// =====================================================
// GOOGLE SHEETS
// =====================================================

// ID da sua planilha Google Sheets
#define SPREADSHEET_ID "1c08SXuW4h0amf_1ew4WJW62ovUtIl2C7F6VsUH7wcbI"

// Nome da aba e célula inicial
#define SHEET_RANGE "Dados!A2"

// =====================================================
// WIFI
// =====================================================

#define WIFI_SSID "VaiTomando"
#define WIFI_PASSWORD "passa4e2carros"

// =====================================================
// ADS1115
// =====================================================

Adafruit_ADS1115 ads;

// =====================================================
// CONTROLE DE TEMPO
// =====================================================

unsigned long ultimoEnvio = 0;

const unsigned long intervalo = 1000;

// =====================================================
// FUNÇÃO PARA OBTER HORÁRIO
// =====================================================

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

// =====================================================
// ENVIA DADOS PARA GOOGLE SHEETS
// =====================================================

void writeData(int16_t valorADS, int angulo)
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
  valueRange.set("values/[0]/[1]", valorADS);
  valueRange.set("values/[0]/[2]", angulo);

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

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  Serial.println();
  Serial.println("Inicializando ADS1115...");

  // ADS1115
  if (!ads.begin())
  {
    Serial.println("Falha ao inicializar o ADS1115!");
    // while (1);
  }

  Serial.println("ADS1115 OK");

  // ===================================================
  // WIFI
  // ===================================================

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

  // ===================================================
  // NTP
  // ===================================================

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

  // ===================================================
  // GOOGLE SHEETS
  // ===================================================

  Serial.println("Inicializando Google Sheets...");

  GSheet.begin(
    CLIENT_EMAIL,
    PROJECT_ID,
    PRIVATE_KEY
  );

  Serial.println("Google Sheets inicializado!");
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
  // Executa a cada 1 segundo
  if (millis() - ultimoEnvio >= intervalo)
  {
    ultimoEnvio = millis();

    // ================================================
    // LEITURA DO ADS1115
    // ================================================

    int16_t adc0;

    adc0 = random(3000, 17600);

    // ================================================
    // CONVERSÃO PARA ÂNGULO
    // ================================================

    int angulo = map(
      adc0,
      3000,
      17600,
      0,
      180
    );

    // Limita entre 0 e 180
    angulo = constrain(
      angulo,
      0,
      180
    );

    // ================================================
    // MOSTRA NO SERIAL
    // ================================================

    Serial.println();
    Serial.println("============================");

    Serial.print("Horario: ");
    Serial.println(getHorario());

    Serial.print("ADC: ");
    Serial.println(adc0);

    Serial.print("Angulo: ");
    Serial.println(angulo);

    // ================================================
    // ENVIA PARA GOOGLE SHEETS
    // ================================================

    writeData(
      adc0,
      angulo
    );
  }

  // Permite que a biblioteca do Google Sheets
  // processe autenticação/comunicação
  GSheet.ready();
}