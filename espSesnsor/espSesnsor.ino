//URL For Sketch Skeleton: https://randomnerdtutorials.com/esp-now-two-way-communication-esp32/
#include <WiFi.h>//to find MAC 
#include <esp_now.h>
#include <esp_wifi.h>
#include <DHT.h>

#define DHT_TYPE 11
#define DHT_PIN 0


//Receiver Address
uint8_t MAC[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
//defininig structured message to send 
typedef struct Data {
  int dist;
} data;

//for peerinfo
esp_now_peer_info_t peerInfo;
//checksum for function
char *success;
//global variable for distance measure
uint8_t * globalDist;

//callback to send data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success";
  } else {
    success = "Delivery Fail";
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

  //Initializes esp_now which has to be done after starting WiFi
  if (esp_now_init() != ESP_OK) {
    Serial.println("ERROR, ESP Init Failed . . .");
    return;
  }
  //registers esp to get transmitted packet data
  if (esp_now_register_send_cb(OnDataSent) != ESP_OK) {
    Serial.println("Packet Registration Error!");
    return;
  }
  //registers peer
  memcpy(peerInfo.peer_addr, MAC, 6);//6 = sizeof(MAC)
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  //adds peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  if (esp_now_send(MAC, globalDist, sizeof(globalDist)) != ESP_OK) {//casts struct elements to uint8_t elements
    Serial.println("Sent with Success");
  } else {
    Serial.println("Error Sending the Data!");
  }

  delay(10000);//10 second delay  
}