#include <WiFi.h>
#include <esp_wifi.h>

uint8_t MAC[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

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
  WiFi.STA.begin();
  Serial.print("MAC: ");
  readMACAddress();
  
  Serial.print("Wifi Channel:");
  Serial.println(WiFi.channel());

  
}

void loop() {
  delay(10000);
}
