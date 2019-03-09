#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

//Map
#define NUM_LEDS 8
float weather[NUM_LEDS][3];

//LEDs
#define DATA_PIN 2
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

int LEDTimer = 0;
int LEDUpdateTime = 25;

//Weather
int WeatherID = 0;
int WeatherTimer = 0;
int WeatherUpdateTime = 180000;

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup() {
  Serial.begin(115200);

  Serial.println("Startup.");
  pixels.begin(); // This initializes the NeoPixel library.

  delay(3000);

  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, pixels.Color(80, 80, 80));
  }
  pixels.show(); // This sends the updated pixel color to the hardware.

  WiFiManager wifiManager;
  //wifiManager.resetSettings();
  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect("Map_Config")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  Serial.println("Connected!");
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 150, 0)); // Moderately bright green color.
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
}

void loop() {
  if (millis() > WeatherTimer) {
    Serial.println(F("Weather Update!"));
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      client.setTimeout(10000);

      if (!client.connect("api.openweathermap.org", 80)) {
        Serial.println(F("Connection failed"));
        return;
      }

      Serial.println(F("Connected!"));
      
       client.println(F("GET /data/2.5/weather?q=ishinomaki&APPID=d84a377a00971e959a35865489638881 HTTP/1.0"));//ishinomaki
      //client.println(F("GET /data/2.5/weather?lat=38.891147&lon=141.561163&APPID=d84a377a00971e959a35865489638881 HTTP/1.0"));//kesennuma
      
      if (client.println() == 0) {
        Serial.println(F("Failed to send request"));
        return;
      }
      // Check HTTP status
      char status[32] = {0};
      client.readBytesUntil('\r', status, sizeof(status));
      if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
        return;
      }

      // Skip HTTP headers
      char endOfHeaders[] = "\r\n\r\n";
      if (!client.find(endOfHeaders)) {
        Serial.println(F("Invalid response"));
        return;
      }

      // Allocate JsonBuffer
      // Use arduinojson.org/assistant to compute the capacity.
      const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 410;
      DynamicJsonBuffer jsonBuffer(capacity);

      // Parse JSON object
      JsonObject& root = jsonBuffer.parseObject(client);
      if (!root.success()) {
        Serial.println(F("Parsing failed!"));
        return;
      }

      // Extract values
      Serial.println(F("Response:"));
      WeatherID = root["weather"][0]["id"].as<int>();
      Serial.println(WeatherID);

      // Disconnect
      client.stop();
    }
    WeatherTimer += WeatherUpdateTime;
  }
  if (millis() > LEDTimer) {
    for (int i = 0; i < NUM_LEDS; i++) {
      long randNumber = random(1000);
      //WeatherID = 771;
      switch (WeatherID) {
        //Thunderstorm
        case 200:
        case 201:
        case 202:
        case 210:
        case 211:
        case 212:
        case 221:
        case 230:
        case 231:
        case 232:
          if(randNumber < 2){
            weather[i][0] = 218;
            weather[i][1] = 73;
            weather[i][2] = 255;
            if(randNumber < 1){
              weather[i][0] = 255;
              weather[i][1] = 255;
              weather[i][2] = 255;
            }
          } else {
            weather[i][0] *= 0.8;
            weather[i][1] *= 0.8;
            weather[i][2] *= 0.8;
          }
          break;
        //Drizzle
        case 300:
        case 301:
        case 302:
        case 310:
        case 311:
        case 312:
        case 313:
        case 314:
        case 321:
          if(randNumber < 3){
              weather[i][0] = 128*.7;
              weather[i][1] = 140*.7;
              weather[i][2] = 255*.7;
            } else {
              weather[i][0] *= 0.97;
              weather[i][1] *= 0.97;
              weather[i][2] *= 0.97;
            }
          break;
        //Rain
        case 500://LIGHT
          if(randNumber < 4){
              weather[i][0] = 70;
              weather[i][1] = 87;
              weather[i][2] = 255;
            } else {
              weather[i][0] *= 0.99;
              weather[i][1] *= 0.99;
              weather[i][2] *= 0.99;
            }
          break;
        case 501://MEDIUM
        case 520:
        case 521:
        if(randNumber < 6){
              weather[i][0] = 70;
              weather[i][1] = 87;
              weather[i][2] = 255;
            } else {
              weather[i][0] *= 0.99;
              weather[i][1] *= 0.99;
              weather[i][2] *= 0.99;
            }
          break;
        case 502://HEAVY
        case 503:
        case 504:
        case 511:
        case 531:
          if(randNumber < 15){
              weather[i][0] = 70;
              weather[i][1] = 87;
              weather[i][2] = 255;
            } else {
              weather[i][0] *= 0.99;
              weather[i][1] *= 0.99;
              weather[i][2] *= 0.99;
            }
          break;
        //Snow
        case 616: // SLEET
        case 615:
        case 611:
        case 612:
        case 602:
          if(randNumber < 10){
              weather[i][0] = 200;
              weather[i][1] = 200;
              weather[i][2] = 200;
            } else {
              weather[i][0] *= 0.99;
              weather[i][1] *= 0.99;
              weather[i][2] *= 0.99;
            }
          break;
        case 620:
        case 621:
        case 622:
        case 600:
        case 601:
          if(randNumber < 3){
              weather[i][0] = 255;
              weather[i][1] = 255;
              weather[i][2] = 255;
            } else {
              weather[i][0] *= 0.999;
              weather[i][1] *= 0.999;
              weather[i][2] *= 0.999;
            }
          break;
        //Atmosphere
        case 701: //MIST
        case 721:
        case 741:
            weather[i][0] = 97;
            weather[i][1] = 133;
            weather[i][2] = 135;
          break;
        case 711: //SMOKE
          weather[i][0] = 91;
            weather[i][1] = 91;
            weather[i][2] = 91;
          break;
        case 731:
        case 751: //DUST
        case 761:
        case 762:
          weather[i][0] = 128;
            weather[i][1] = 128;
            weather[i][2] = 110;
          break;
        case 771:// STORM
        case 781:
          weather[i][0] = 166;
            weather[i][1] = 62;
            weather[i][2] = 255;
          break;
        //Clear
        case 800:
          weather[i][0] = 0;
            weather[i][1] = 0;
            weather[i][2] = 0;
          break;
        //Clouds
        case 801:
           weather[i][0] = 30;
           weather[i][1] = 30;
           weather[i][2] = 30;
          break;
        case 802:
          weather[i][0] = 50;
           weather[i][1] = 50;
           weather[i][2] = 50;
          break;
        case 803:
          weather[i][0] = 70;
          weather[i][1] = 70;
          weather[i][2] = 70;
          break;
        case 804:
          weather[i][0] = 90;
           weather[i][1] = 90;
           weather[i][2] = 90;
          break;
        default:
          weather[i][0] = 0;
            weather[i][1] = 0;
            weather[i][2] = 0;
          break;
      }
    }
    for (int i = 0; i < NUM_LEDS; i++) {
      pixels.setPixelColor(i, pixels.Color(
        weather[i][0],
        weather[i][1],
        weather[i][2]
        ));
    }
    pixels.show(); // This sends the updated pixel color to the hardware.
    LEDTimer += LEDUpdateTime;
  }
}
