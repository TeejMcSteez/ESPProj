//URL For Sketch Skeleton: https://randomnerdtutorials.com/esp-now-two-way-communication-esp32/
#include <WiFi.h>//to find MAC 
#include <esp_now.h>
#include <esp_wifi.h>

const int TRIGGER_PIN = 23;
const int ECHO_PIN = 0;

//Receiver Address
uint8_t MAC[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
//for peerinfo
esp_now_peer_info_t peerInfo;
//checksum for function
char *success;
//global variable for distance measure
uint8_t globalDist;

//callback to send data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sent with Success" : "Failed to send");
  if (status == 0) {
    success = "Sent with Success";
  } else {
    success = "Failed to send";
  }
}

int distSensor() {
  //variables to calculate distance
  long duration;
  float distance;
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  //CALCULATE DISTANCE
  //reads wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  /*
  Problem with distance calculation as I want to use an integer however a floating point value defaults
  to 0 therefore the output will always be 0. Have to cast the result of the calculation (float) to the distance (int)
  */
  //calculates distance
  distance = duration * 0.034/2;//duration (in ms) * V/K
  return (int) distance; 
}

void setup() {
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  //Initializes esp_now which has to be done after starting WiFi!
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
  //gets distance of sensor to send to receiver
  globalDist = distSensor();
  Serial.print("Global Distance: ");
  Serial.println(globalDist);
  //stores function call in result for error checking and follows API documentation
  esp_err_t result = esp_now_send(MAC, &globalDist, sizeof(globalDist)); 

  if (result != ESP_OK) {
    Serial.println("Sent with Success");
  } else {
    Serial.println("Error W/ Reciever accepting the Data!");
    Serial.println("Check MAC Address or callback functions to ensure correct addressing and proper data handling");
  }

  delay(10000);//10 second delay  
}