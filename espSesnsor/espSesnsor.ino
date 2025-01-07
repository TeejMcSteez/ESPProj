#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

const int TRIGGER_PIN = 23;
const int ECHO_PIN = 16;
const uint8_t WIFI_CHANNEL = 1;  // Match this with receiver

// Receiver MAC Address - double check this matches your receiver's MAC
uint8_t receiverMac[] = {0x24, 0xDC, 0xC3, 0x44, 0xDC, 0x70};

esp_now_peer_info_t peerInfo;
char *success;
uint8_t globalDist;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nPacket sent to: ");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", mac_addr[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.print("\nStatus: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failed");
}

float distSensor() {
    // Clear the trigger pin
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    
    // Send the trigger pulse
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    
    // Read the echo pin
    long duration = pulseIn(ECHO_PIN, HIGH);
    
    // Calculate distance
    float distance = (duration * 0.034) / 2;
    
    Serial.print("Raw distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    // Constrain the distance value
    if (distance < 1) {
        Serial.println("Distance < 1cm, returning 0");
        return 0;
    } else if (distance > 255) {
        Serial.println("Distance > 255cm, returning 255");
        return 255;
    } else if (distance > 200) {
        Serial.println("Distance > 200cm, returning 200");
        return 200;
    }
    return distance;
}

void initESPNow() {
    WiFi.mode(WIFI_STA);
    
    // Print our MAC address for debugging
    Serial.print("Sender MAC: ");
    Serial.println(WiFi.macAddress());
    
    // Set WiFi channel
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
    
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    
    // Register callback
    esp_now_register_send_cb(OnDataSent);
    
    // Register peer
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, receiverMac, 6);
    peerInfo.channel = WIFI_CHANNEL;
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
    
    Serial.println("ESP-NOW initialized successfully");
}

void setup() {
    Serial.begin(115200);
    
    // Configure pins
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    // Initialize ESP-NOW
    initESPNow();
}

void loop() {
    // Get distance reading
    globalDist = static_cast<uint8_t>(distSensor());
    
    Serial.print("Sending distance: ");
    Serial.println(globalDist);
    
    // Only send if distance is less than 200
    if (globalDist < 200) {
        esp_err_t result = esp_now_send(receiverMac, &globalDist, sizeof(globalDist));
        
        if (result == ESP_OK) {
            Serial.println("Sent successfully");
        } else {
            Serial.println("Send failed! Error code: ");
            Serial.println(result);
        }
    }
    
    delay(1000);  // Wait 1 second before next reading
}