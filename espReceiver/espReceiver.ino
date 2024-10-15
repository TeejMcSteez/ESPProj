#include <WiFi.h>
#include <esp_now.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

const char* ssid = "World Wide Web";
const char* password = "ablecapital114";

AsyncWebServer server(80);

const int LED_PIN_1 = 13; // Blue LED
const int LED_PIN_2 = 0;  // Red LED

uint8_t incomingDist = 0;
uint8_t Distances[5];
//to index last known distance values
int i = 0;
//for time logging
time_t now;
struct tm timeInfo;

char timeString[64];

void OKLight() {
  digitalWrite(LED_PIN_1, HIGH);
  delay(500);
  digitalWrite(LED_PIN_1, LOW);
}

void OnDataRecv(const uint8_t* mac_addr, const uint8_t* data, int len) {
  if (len == sizeof(incomingDist)) {
    memcpy(&incomingDist, data, sizeof(incomingDist));
    Serial.print("Distance Received: ");
    Serial.println(incomingDist);
    checkDist(incomingDist);
    Distances[i] = incomingDist;
    if (i == 4) {
      i = 0;
    } else {
      i++;
    }
  } else {
    Serial.println("Received data of unexpected length");
  }
}

void checkDist(uint8_t dist) {
  uint8_t threshold = 103; // Threshold distance
  if (dist < threshold) {
    Serial.println("Signature Detected Close to Sensor");
    digitalWrite(LED_PIN_2, HIGH);
    //For logging last time entered
    now = time(nullptr);
    localtime_r(&now, &timeInfo);
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeInfo);
    delay(500);
    digitalWrite(LED_PIN_2, LOW);
  } 
}

void printWifiStatus() {
  Serial.print("Connected to: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);

  // Initialize Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected.");
  printWifiStatus();

  // Start mDNS service
  if (!MDNS.begin("room")) {
    Serial.println("Error starting mDNS");
    return;
  }
  Serial.println("\nmDNS responder started");

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register receive callback
  esp_err_t cbResult = esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  if (cbResult == ESP_OK) {
    Serial.println("Receive callback registered successfully");
  } else {
    Serial.print("Receive callback registration failed: ");
    Serial.println(cbResult);
    // Handle error accordingly
  }
  // Initialize Async Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    String html = "<!DOCTYPE HTML><html><head>"
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
            "<meta http-equiv='refresh' content='5'>"
              "<style>"
                "body { margin: 0; font-family: Arial, sans-serif; background-color: #000; }"
                ".container { display: flex; flex-direction: column; align-items: center; justify-content: center; min-height: 100vh; }"
                ".card { background-color: #f0f2f548; padding: 20px; margin: 10px; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); width: 90%; max-width: 500px; }"
                ".card h1 { font-size: 24px; color: #ff0808a4; margin-bottom: 15px; }"
                ".record { display: flex; flex-direction: column; align-items: flex-start; }"
                ".record h2 { font-size: 20px; color: #ff0808a4; margin: 5px 0; }"
              "</style>"
              "</head><body>"
                "<div class='container'>"
                  "<div class='card'>"
                    "<h1>Incoming Distance Value:</h1>"
                    "<h1>" + String(incomingDist) + " cm</h1>"
                  "</div>"
                  "<div class='card'>"
                  "<h1>Distance Records:</h1>"
                    "<div class ='record'>"
                      "<h2>Distance Record 1: " + String(Distances[0]) + " cm</h2>"
                      "<h2>Distance Record 2: " + String(Distances[1]) + " cm</h2>"
                      "<h2>Distance Record 3: " + String(Distances[2]) + " cm</h2>"
                      "<h2>Distance Record 4: " + String(Distances[3]) + " cm</h2>"
                      "<h2>Distance Record 5: " + String(Distances[4]) + " cm</h2>"
                      "<h2>Last Entry: " + String(timeString) + "</h2>"
                    "</div>"
                  "</div>"
                "</body></html>";//END OF HTML
    request->send(200, "text/html", html);
  });

  server.begin();
  Serial.println("Web server started.");
  //Time logging setup
  configTime(-18000, 3600, "pool.ntp.org", "time.nist.gov");//-18000 is the UTC Eastern Standard Time offset in seconds and 3600 is the offset for daylight savings time and 0 is no daylight savings
  if (!getLocalTime(&timeInfo)) {
    Serial.println("Failed to obtain time data");
  }
  Serial.println("Time Synced");

  OKLight();
  Serial.println("Setup completed.");
}

void loop() {
  //nothing needed everythings Async
}
