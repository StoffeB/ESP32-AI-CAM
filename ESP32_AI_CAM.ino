#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

WebServer server(80);

void handleRoot(){
  server.send(200,"text/html",
  "<h1>ESP32 AI CAM</h1><img src='http://"+WiFi.localIP().toString()+":81/stream' width='640'>");
}

void handleCapture(){
  camera_fb_t * fb = esp_camera_fb_get();
  if(!fb){server.send(500,"text/plain","capture failed");return;}
  server.send_P(200,"image/jpeg",(const char*)fb->buf,fb->len);
  esp_camera_fb_return(fb);
}

void setup(){
 Serial.begin(115200);
 camera_config_t config;
 config.ledc_channel=LEDC_CHANNEL_0;
 config.ledc_timer=LEDC_TIMER_0;
 config.pin_d0=5; config.pin_d1=18; config.pin_d2=19; config.pin_d3=21;
 config.pin_d4=36; config.pin_d5=39; config.pin_d6=34; config.pin_d7=35;
 config.pin_xclk=0; config.pin_pclk=22; config.pin_vsync=25; config.pin_href=23;
 config.pin_sscb_sda=26; config.pin_sscb_scl=27; config.pin_pwdn=32; config.pin_reset=-1;
 config.xclk_freq_hz=20000000;
 config.pixel_format=PIXFORMAT_JPEG;
 config.frame_size=FRAMESIZE_VGA;
 config.jpeg_quality=10;
 config.fb_count=2;

 esp_camera_init(&config);

 WiFi.begin(ssid,password);
 while(WiFi.status()!=WL_CONNECTED){delay(500);Serial.print(".");}
 Serial.println(WiFi.localIP());

 server.on("/",handleRoot);
 server.on("/capture",handleCapture);
 server.begin();
}

void loop(){server.handleClient();}
