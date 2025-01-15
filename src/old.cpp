// #include <Arduino.h>
// #include <FS.h>
// #include <WiFi.h>
// #define CAMERA_MODEL_AI_THINKER
// #include "camera_pins.h"
// #include <esp_camera.h>
// #include "cam_server_handle.h"

// void camera_init();
// void wifi_init();
// void device_server_init();
// void stream_server_init();

// AsyncWebServer device_server(80);
// AsyncWebServer stream_server(81);

// const char *ssid = "ellem";
// const char *pass = "12345678";

// void setup() {
//   Serial.begin(115200);
//   delay(5000);
//   Serial.println("Inicio");
//   camera_init();
//   delay(5000);
//   wifi_init();
//   //device_server_init();
//   stream_server_init();
//   Serial.println("Setup");
// }

// void loop() {
//   delay(1000);
//   // Serial.println("Loop");
// }

// void camera_init(){
//   camera_config_t config;
//   config.ledc_channel = LEDC_CHANNEL_0;
//   config.ledc_timer = LEDC_TIMER_0;
//   config.pin_d0 = Y2_GPIO_NUM;
//   config.pin_d1 = Y3_GPIO_NUM;
//   config.pin_d2 = Y4_GPIO_NUM;
//   config.pin_d3 = Y5_GPIO_NUM;
//   config.pin_d4 = Y6_GPIO_NUM;
//   config.pin_d5 = Y7_GPIO_NUM;
//   config.pin_d6 = Y8_GPIO_NUM;
//   config.pin_d7 = Y9_GPIO_NUM;
//   config.pin_xclk = XCLK_GPIO_NUM;
//   config.pin_pclk = PCLK_GPIO_NUM;
//   config.pin_vsync = VSYNC_GPIO_NUM;
//   config.pin_href = HREF_GPIO_NUM;
//   config.pin_sccb_sda = SIOD_GPIO_NUM;
//   config.pin_sccb_scl = SIOC_GPIO_NUM;
//   config.pin_pwdn = PWDN_GPIO_NUM;
//   config.pin_reset = RESET_GPIO_NUM;
//   config.xclk_freq_hz = 20000000;
//   config.frame_size = FRAMESIZE_VGA;
//   config.pixel_format = PIXFORMAT_JPEG; // for streaming
//   //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
//   config.grab_mode = CAMERA_GRAB_LATEST;
//   config.fb_location = CAMERA_FB_IN_PSRAM;
//   config.jpeg_quality = 10;
//   config.fb_count = 2;

//   // if (psramFound()) {
//   //   config.frame_size = FRAMESIZE_UXGA;
//   //   config.jpeg_quality = 10;
//   //   config.fb_count = 2;
//   // } else {
//   //   config.frame_size = FRAMESIZE_SVGA;
//   //   config.jpeg_quality = 12;
//   //   config.fb_count = 1;
//   // }
//   // Camera init
//   esp_err_t err = esp_camera_init(&config);
//   if (err != ESP_OK) {
//     Serial.printf("Camera init failed with error 0x%x", err);
//     // ESP.restart();
//      return;
//   }
//   Serial.println("Camera inicializada com sucesso");
//   sensor_t *s = esp_camera_sensor_get();
//   s->set_framesize(s, FRAMESIZE_QVGA); 

// }

// void wifi_init(){
//   WiFi.begin(ssid, pass);
//   WiFi.setSleep(false);
//   while(WiFi.status() != WL_CONNECTED){
//     Serial.println("Connecting to WiFi...");
//     delay(500);
//   }
//   Serial.print("Connected to WiFi. IP:");
//   Serial.println(WiFi.localIP());

// }

// void stream_server_init(){
//   stream_server.on("/stream", HTTP_GET, streamJpg);
//   stream_server.on("/sendjpg", HTTP_GET, sendJpg);

//   stream_server.begin();
// }