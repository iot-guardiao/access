#include <ESP32QRCodeReader.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "LITTORAL";
const char *password = "12345678";

const char *url = "http://192.168.5.182:8000/";

// See available models on README.md or ESP32CameraPins.h
ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);

String qrPayload = ""; // Armazena o conteúdo do QR code
bool qrPayloadReady = false; // Flag para indicar se o QR code foi lido e está pronto para envio

String httpGETRequest(const char *serverName)
{
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);

  // Send HTTP GET request
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

  // Free resources
  http.end();

  return payload;
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
        Serial.print("Invalid QR Code: ");
        Serial.println((const char *)qrCodeData.payload);
        qrPayload = "";         // Limpa o payload inválido
        qrPayloadReady = false; // Flag não pronta
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void onHttpRequestTask(void *pvParameters)
{
  unsigned long previousMillis = 0;
  const long interval = 5000;

  while (true)
  {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval && qrPayloadReady)
    {
      // Check WiFi connection status
      if (WiFi.status() == WL_CONNECTED)
      {
        String requestUrl = String(url) + "?qr="  + "benjamin";
        Serial.println("Send:" + requestUrl);
        String response = httpGETRequest(requestUrl.c_str());
        Serial.println("HTTP Response: " + response);

        qrPayload = "";         // Limpa o payload após o envio
        qrPayloadReady = false; // Reseta a flag após o envio
      }
      else
      {
        Serial.println("WiFi Disconnected");
      }

      previousMillis = currentMillis;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
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

  // Create tasks
  xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, NULL);
  xTaskCreate(onHttpRequestTask, "onHttpRequest", 4 * 1024, NULL, 4, NULL);
}

void loop()
{
  // O loop está vazio porque todas as operações são realizadas nas tasks.
}
