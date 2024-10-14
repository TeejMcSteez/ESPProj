//URL For Sketch Skeleton: https://randomnerdtutorials.com/esp-now-two-way-communication-esp32/
/*
TODO:
Network communication works 
Outputting to the LED lights doesnt work prob wiring issue 
See what I can do about other packet sizes or handling less packet loss.
Maybe implement a small HTTP server also to see the values or notification logs on a display
*/
#include <WiFi.h>//to find MAC 
#include <esp_now.h>
#include <esp_wifi.h>

uint8_t MAC[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//Not currently needed for receiver

const int LED_PIN_1 = 13;//blue
const int LED_PIN_2 = 0;//red

esp_now_peer_info_t peerInfo;

uint8_t incomingDist;

void OnDataRecv(const uint8_t * MAC, const uint8_t *dist, int len) {
  incomingDist = * dist;
  Serial.print("Bytes Received:");
  Serial.println(len);
  Serial.print("From MAC: ");
  Serial.print(*MAC + 0);
  Serial.print(":");
  Serial.print(*MAC + 1);
  Serial.print(":");
  Serial.print(*MAC + 2);
  Serial.print(":");
  Serial.print(*MAC + 3);
  Serial.print(":");
  Serial.print(*MAC + 4);
  Serial.print(":");
  Serial.println(*MAC + 5);
  Serial.print("Distance Received:");
  Serial.println(incomingDist);

  checkDist(incomingDist);
}
//Should miss 4 packets while executing 
void checkDist(uint8_t dist) {
  uint8_t x = 90;//some distance assign when testing
  if (dist >= x) {
    Serial.println("GREATER THAN OR =");
    digitalWrite(LED_PIN_1, HIGH);
    delay(500);
    digitalWrite(LED_PIN_1, LOW);
  } else {
    Serial.println("LESS THAN");
    digitalWrite(LED_PIN_2, HIGH);
    delay(500);
    digitalWrite(LED_PIN_2, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ERROR Initializing ESP-NOW");
    return;
  }

  // Registering Peer
  memcpy(peerInfo.peer_addr, MAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Intitializtion Successful Starting Loop");

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {

  delay(500);

}

