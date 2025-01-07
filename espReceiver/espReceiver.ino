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
uint8_t distanceRecords[5]; //Stores last 5 distances measured
int distanceIndex = 0;
time_t nowTime;
struct tm timeInfo;
char timeString[64];
char timeRecords[5][64];

// Updated callback function to match new ESP-NOW API
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) { // Instead of using a uint8_t pointer to store MAC use ESP formatted input for data
    if (len == sizeof(incomingDist)) {
        memcpy(&incomingDist, data, sizeof(incomingDist));
        Serial.print("Distance Received: ");
        Serial.println(incomingDist);
        
        // Print sender MAC address
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
                recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);
        Serial.print("Received from: ");
        Serial.println(macStr);
        
        checkDist(incomingDist);
    } else {
        Serial.println("Received data of unexpected length");
    }
}

void OKLight() {
    digitalWrite(LED_PIN_1, HIGH);
    delay(500);
    digitalWrite(LED_PIN_1, LOW);
}

void indexCount() { // Instead of using if statements call a function which takes the remainder of dividing the sum so if at max will remainder to 0
    distanceIndex = (distanceIndex + 1) % 5;
}

void checkDist(uint8_t dist) {
    uint8_t threshold = 140;
    if (dist < threshold) {
        Serial.println("Signature Detected Close to Sensor");
        digitalWrite(LED_PIN_2, HIGH);
        
        nowTime = time(nullptr);
        localtime_r(&nowTime, &timeInfo);
        strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeInfo);
        strftime(timeRecords[distanceIndex], sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeInfo);
        distanceRecords[distanceIndex] = incomingDist;
        indexCount();
        delay(500);
        digitalWrite(LED_PIN_2, LOW);
    }
}

void initESPNow() {
    // Set device as a Wi-Fi Station (and access point for better connectivity)
    WiFi.mode(WIFI_AP_STA);

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register callback function
    esp_err_t result = esp_now_register_recv_cb(OnDataRecv);
    if (result == ESP_OK) {
        Serial.println("Callback registered successfully");
    } else {
        Serial.printf("Failed to register callback: %d\n", result);
    }
}

void setup() {
    Serial.begin(115200);
    
    pinMode(LED_PIN_1, OUTPUT);
    pinMode(LED_PIN_2, OUTPUT);

    // Initialize Wi-Fi
    WiFi.begin(ssid, password);

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConnected.");
    
    // Print our own MAC address for reference
    Serial.print("Receiver MAC Address: ");
    Serial.println(WiFi.macAddress());
    
    // Initialize ESP-NOW
    initESPNow();

    // Start mDNS
    if (!MDNS.begin("room")) {
        Serial.println("Error starting mDNS");
        return;
    }

    // Initialize Web Server
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        String html = "<!DOCTYPE HTML><html><head>"
                "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                "<meta http-equiv='refresh' content='5'>"
                "<style>"
                    "body { margin: 0; font-family: Arial, sans-serif; background-color: #000; }"
                    ".container { display: flex; flex-direction: column; align-items: center; justify-content: center; min-height: 100vh; }"
                    ".card { background-color: #f0f2f548; padding: 20px; margin: 10px; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); width: 90%; max-width: 500px; }"
                    ".card h1 { font-size: 24px; color: #FFF; margin-bottom: 15px; }"
                    ".record { display: flex; flex-direction: column; align-items: flex-start; }"
                    ".record h2 { font-size: 20px; color: #FFF; margin: 5px 0; }"
                "</style>"
                "</head><body>"
                    "<div class='container'>"
                        "<div class='card'>"
                            "<h1>Incoming Distance Value:</h1>"
                            "<h1>" + String(incomingDist) + " cm</h1>"
                        "</div>"
                        "<div class='card'>"
                            "<h1>Distance Records:</h1>"
                            "<div class='record'>"
                                "<h2>Distance Record 1: " + String(distanceRecords[0]) + " cm" +" At " + String(timeRecords[0]) + "</h2>"
                                "<h2>Distance Record 2: " + String(distanceRecords[1]) + " cm" +" At " + String(timeRecords[1]) + "</h2>"
                                "<h2>Distance Record 3: " + String(distanceRecords[2]) + " cm" +" At " + String(timeRecords[2]) + "</h2>"
                                "<h2>Distance Record 4: " + String(distanceRecords[3]) + " cm" +" At " + String(timeRecords[3]) + "</h2>"
                                "<h2>Distance Record 5: " + String(distanceRecords[4]) + " cm" +" At " + String(timeRecords[4]) + "</h2>"
                            "</div>"
                        "</div>"
                        "<div class='card'>"
                            "<div class='record'>"
                                "<h1>Last Signature: " + String(timeString) + "</h1>"
                            "</div>"
                        "</div>"
                    "</div>"
                "</body></html>";
        request->send(200, "text/html", html);
    });

    server.begin();
    
    // Configure time
    configTime(-18000, 3600, "pool.ntp.org", "time.nist.gov");
    if (!getLocalTime(&timeInfo)) {
        Serial.println("Failed to obtain time data");
    }
    Serial.println("Time Synced");

    OKLight();
    Serial.println("Setup completed.");
}

void loop() {
    // Monitor WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost. Reconnecting...");
        WiFi.begin(ssid, password);
    }
}