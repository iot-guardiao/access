#include <ESP32QRCodeReader.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "Jordana";
const char *password = "11335577";

const char *url = "http://192.168.37.141:8000/agendamentos/verificar/Leg01/";

// Pinos dos LEDs
const int ledRedPin = 12;
const int ledBluePin = 13;

// Fila para comunicação entre tasks
QueueHandle_t ledQueue;

// Estrutura para mensagens na fila
enum LedState { LED_RED, LED_BLUE };

// Configuração da câmera
ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);

String qrPayload = ""; // Armazena o conteúdo do QR code
bool qrPayloadReady = false; // Flag para indicar se o QR code foi lido e está pronto para envio

String httpGETRequest(const char *serverName)
{
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);

  // Envia a requisição HTTP GET
  int httpResponseCode = http.GET();

  String payload = "--";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  // Libera recursos
  http.end();

  return payload;
}

String urlEncode(String str)
{
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++)
  {
    c = str.charAt(i);
    if (isalnum(c))
    {
      encodedString += c;
    }
    else
    {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9)
      {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9)
      {
        code0 = c - 10 + 'A';
      }
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
  }
  return encodedString;
}

void onQrCodeTask(void *pvParameters)
{
  struct QRCodeData qrCodeData;

  while (true)
  {
    if (reader.receiveQrCode(&qrCodeData, 100))
    {
      Serial.println("Found QRCode");
      if (qrCodeData.valid)
      {
        qrPayload = String((const char *)qrCodeData.payload);
        qrPayloadReady = true; // Sinaliza que há um novo QR code pronto
        Serial.print("Payload: ");
        Serial.println(qrPayload);
      }
      else
      {
        Serial.println("Invalid QR Code");
        qrPayload = "";
        qrPayloadReady = false;
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void onHttpRequestTask(void *pvParameters)
{
  while (true)
  {
    if (qrPayloadReady && WiFi.status() == WL_CONNECTED)
    {
      String requestUrl = String(url) + urlEncode(qrPayload) + "/";
      Serial.println("Send: " + requestUrl);
      String response = httpGETRequest(requestUrl.c_str());
      Serial.println("HTTP Response: " + response);

      // Determina qual LED acender com base na resposta
      LedState ledState = (response == "true") ? LED_BLUE : LED_RED;
      xQueueSend(ledQueue, &ledState, portMAX_DELAY);

      qrPayload = "";         // Limpa o payload após o envio
      qrPayloadReady = false; // Reseta a flag após o envio
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void ledControlTask(void *pvParameters)
{
  LedState ledState;

  while (true)
  {
    if (xQueueReceive(ledQueue, &ledState, portMAX_DELAY))
    {
      if (ledState == LED_BLUE)
      {
        digitalWrite(ledRedPin, LOW);
        digitalWrite(ledBluePin, HIGH);
      }
      else if (ledState == LED_RED)
      {
        digitalWrite(ledBluePin, LOW);
        digitalWrite(ledRedPin, HIGH);
      }

      
      vTaskDelay(5000 / portTICK_PERIOD_MS);

      
      digitalWrite(ledRedPin, LOW);
      digitalWrite(ledBluePin, LOW);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  reader.setup();
  reader.beginOnCore(1);

  // Configuração dos pinos dos LEDs
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledBluePin, OUTPUT);

  // Criação da fila
  ledQueue = xQueueCreate(10, sizeof(LedState));

  // Criação das tasks
  xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, NULL);
  xTaskCreate(onHttpRequestTask, "onHttpRequest", 4 * 1024, NULL, 4, NULL);
  xTaskCreate(ledControlTask, "ledControl", 4 * 1024, NULL, 2, NULL);
}

void loop()
{
  // O loop está vazio porque todas as operações são realizadas nas tasks.
}
