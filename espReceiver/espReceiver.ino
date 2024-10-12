//URL For Sketch Skeleton: https://randomnerdtutorials.com/esp-now-two-way-communication-esp32/
#include <WiFi.h>//to find MAC 
#include <esp_now.h>
#include <esp_wifi.h>

uint8_t MAC[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

esp_now_peer_info_t peerInfo;

uint8_t incomingDist;

void OnDataRecv(const uint8_t * MAC, const uint8_t * dist, int len) {
  memcpy(&dist, dist, sizeof(dist));
  incomingDist = * dist;
  Serial.print("Bytes Received:");
  Serial.println(len);
  Serial.print("Distance Received:");
  Serial.println(* dist);
  checkDist(incomingDist);
}

void checkDist(uint8_t dist) {
  int x;//some distance assign when testing
  if (dist < x) {
    //output 
  } else {
    //output
  }
}

//to find MAC
void readMACAddress(){
  uint8_t baseMAC[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMAC);
  if (ret == ESP_OK) {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n", baseMAC[0], baseMAC[1], baseMAC[2], baseMAC[3], baseMAC[4], baseMAC[5]);
  } else {
    Serial.println("Failed to read MAC address");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  //code to find MAC
  // WiFi.STA.begin();
  // Serial.print("MAC: ");
  // readMACAddress();

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

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {

  delay(5000);

}

