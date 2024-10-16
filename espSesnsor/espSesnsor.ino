//URL For Sketch Skeleton: https://randomnerdtutorials.com/esp-now-two-way-communication-esp32/
#include <WiFi.h>//to find MAC 
#include <esp_now.h>
#include <esp_wifi.h>

const int TRIGGER_PIN = 23;
const int ECHO_PIN = 16;

//Receiver Address
uint8_t MAC[] = {0x24,0xDC,0xC3,0x44,0xDC,0x70};
//for peerinfo
esp_now_peer_info_t peerInfo;
//checksum for function
char *success;
//global variable for distance measure
uint8_t globalDist;
long duration;
float distance;

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

float distSensor() {
  duration = 0;
  //variables to calculate distance
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  //CALCULATE DISTANCE
  //reads wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  Serial.print("Duration: ");
  Serial.println(duration);

  //calculates distance formula
  distance = (duration * 0.034)/2;//formula = D * 0.034/2 and * 100 is to convert into cm
  
  if (distance < 1) {
    Serial.println("Distance is < 1, returning 0");
    return 0;
  } else if (distance > 255) {
    Serial.println("Distance > 255, returning 255");
    return 255;
  } else if (distance > 200) {
    Serial.println("Distance > 200 returning 200");
    return 200;
  }
   else {
    return static_cast<uint8_t>(distance);
  } 
}

void setup() {
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(7,WIFI_SECOND_CHAN_NONE);//sets wifi channel to the same as receiver connection
  esp_wifi_set_promiscuous(false);

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
  //resets global distance variable for every distance
  globalDist = 0;
  //gets distance of sensor to send to receiver
  globalDist = distSensor();
  delay(2);

  Serial.print("Global Distance: ");
  Serial.println(globalDist);
 if (globalDist < 200) {
    //stores function call in result for error checking and follows API documentation
    esp_err_t result = esp_now_send(MAC, &globalDist, sizeof(globalDist)); 

    if (result != ESP_OK) {
      Serial.println("Sent with Success");
    } else {
      Serial.println("Error W/ Reciever accepting the Data!");
      Serial.println("Check MAC Address or callback functions to ensure correct addressing and proper data handling");
    }
 }

  delay(200);//.498 second delay  
}