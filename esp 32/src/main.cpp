#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoBLE.h>
#include <TaskScheduler.h>
#include <ArduinoJson.h>
#include <LoRa.h>

bool is_broadcast(uint8_t* mac){
  return (mac[0]==0xff&&
          mac[1]==0xff&&
          mac[2]==0xff&&
          mac[3]==0xff&&
          mac[4]==0xff&&
          mac[5]==0xff
          );
}

bool i_m_gateway=false;

// Funzione per convertire una stringa MAC in un array di byte
void macStrToByteArray(const String &macStr, uint8_t *macArray) {
    // Controllo se la lunghezza della stringa MAC è corretta
    if (macStr.length() != 17) {
        Serial.println("Formato MAC non valido");
        return;
    }

    char hexNum[3]; // Buffer temporaneo per memorizzare coppie di cifre esadecimali
    for (int i = 0; i < 6; ++i) {
        hexNum[0] = macStr.charAt(i * 3);
        hexNum[1] = macStr.charAt(i * 3 + 1);
        hexNum[2] = '\0'; // Terminatore di stringa per assicurare una corretta conversione

        // Converti la coppia di cifre esadecimali in un byte e memorizzalo nell'array
        macArray[i] = strtoul(hexNum, NULL, 16);
    }
}

// REPLACE WITH THE MAC Address of your receiver 
uint8_t remote_ble[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

uint8_t remote_wifi_next[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t remote_wifi_prec[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Variable to store if sending data was successful
String success;

typedef struct struct_message {
    char type[20];
    char text[100]; 
    char source[30];
    char dest[30]; 
} struct_message;

String temp;

// Create a struct_message to hold incoming data
struct_message incomingReadings;

// Create a struct_message to send data
struct_message message;

esp_now_peer_info_t peerInfo;
esp_now_peer_info_t peerInfo2;
esp_now_peer_info_t peerInfo3;

Scheduler ts;

void readBLE();
void sendBLE();

Task ReadBLE(10000,TASK_FOREVER,&readBLE,&ts,true);
//Task SendBLE(100,TASK_IMMEDIATE,&sendBLE,&ts,true);

bool data_ready=false;
JsonDocument MacAddress;

#define TEMPERATURE_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SATURATION_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define HEARTBEAT_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a0"

bool send_data_to(uint8_t* dest){

  if(!is_broadcast(dest)){
    esp_err_t result = esp_now_send(dest, (uint8_t *) &message, sizeof(message));
    if (result == ESP_OK) {
    Serial.println("Sent with success");
    Serial.println();
    return true;
    }
    else {
      Serial.println("Error sending the data");
      Serial.println();
      return false;
    }
  }
  else{
    Serial.println("sarebbe Broadcast beddu");
    return false;
  }
}

void sendBLE(){
  if(MacAddress.size()==0&&data_ready){
    return;
  }
  String BLEmessage;
  serializeJson(MacAddress, BLEmessage);

  strcpy(message.type,String("BraceletData").c_str());
  message.type[String("BraceletData").length()]='\0';

  strcpy(message.source,WiFi.macAddress().c_str());
  message.source[WiFi.macAddress().length()]='\0';

  strcpy(message.text,BLEmessage.c_str());
  message.text[BLEmessage.length()]='\0';

  strcpy(message.dest,String("1:1:1:1:1:1").c_str());
  message.dest[String("1:1:1:1:1:1").length()]='\0';
  
  int retry=0;
  while(!send_data_to(remote_wifi_prec)){
    retry++;
        if(retry==10)   
          break;
  }

  data_ready=false;
  MacAddress.clear();
}

void readBLE(){
  BLE.scanForName("Braccialetto");

  for(int i = 0; i < 50; i++){
    
    delay(50);

    BLEDevice peripheral = BLE.available();
    if(peripheral){
      //se peripheral.address() è già presente in MacAddressArray non fare nulla
      if(MacAddress.containsKey(peripheral.address())){
        //Serial.println("again");
        peripheral.disconnect();
        continue;
      }
      else{
        Serial.println("Address: " + peripheral.address());
        BLE.stopScan();

        if (peripheral.connect()) {
          Serial.println("Connected");
        } else {
          Serial.println("Failed to connect!");
          BLE.scanForName("Braccialetto");
          peripheral.disconnect();
          continue;
        }

        // discover peripheral attributes
        Serial.println("Discovering attributes ...");
        if (peripheral.discoverAttributes()) {
          Serial.println("Attributes discovered");
        } else {
          Serial.println("Attribute discovery failed!");
          peripheral.disconnect();
          BLE.scanForName("Braccialetto");
          continue;
        }
        BLECharacteristic TempCharacteristic = peripheral.characteristic(TEMPERATURE_CHARACTERISTIC_UUID);
        BLECharacteristic SaturationCharacteristic = peripheral.characteristic(SATURATION_CHARACTERISTIC_UUID);
        BLECharacteristic HeartbeatCharacteristic = peripheral.characteristic(HEARTBEAT_CHARACTERISTIC_UUID);

        TempCharacteristic.read();
        SaturationCharacteristic.read();
        HeartbeatCharacteristic.read();

        int tempLength = TempCharacteristic.valueLength();
        int satLength = SaturationCharacteristic.valueLength();
        int heartLength = HeartbeatCharacteristic.valueLength();

        uint8_t tempValue[tempLength];
        uint8_t satValue[satLength];
        uint8_t heartValue[heartLength];

        if (tempLength > 0) {
          TempCharacteristic.readValue(tempValue, tempLength);
          tempValue[tempLength] = '\0';
          Serial.print("Temperature: ");
          Serial.println((char*)tempValue);
        }

        if (satLength > 0) {
          SaturationCharacteristic.readValue(satValue, satLength);
          satValue[satLength] = '\0';
          Serial.print("Saturation: ");
          Serial.println((char*)satValue);
        }

        if (heartLength > 0) {
          HeartbeatCharacteristic.readValue(heartValue, heartLength);
          heartValue[heartLength] = '\0';
          Serial.print("Heartbeat: ");
          Serial.println((char*)heartValue);
        }

        char buffer[100];
        snprintf(buffer, sizeof(buffer), "{Temperature: %s, Saturation: %s, Heartbeat: %s}", 
          (char*)tempValue, (char*)satValue, (char*)heartValue);
        
        MacAddress[peripheral.address()]=buffer;
        BLE.scanForName("Braccialetto");
        peripheral.disconnect();
      }
      
      peripheral.disconnect();
    }

    BLE.scanForName("Braccialetto");
    peripheral.disconnect();
  }
  
  data_ready=true;
  serializeJsonPretty(MacAddress, Serial);
  sendBLE();
}


// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("type:  ");
  Serial.println(incomingReadings.type);
  Serial.print("dest:  ");
  Serial.println(incomingReadings.dest);
  Serial.print("text:  ");
  Serial.println(incomingReadings.text);

  Serial.println("");

  if(String(incomingReadings.type).equals("new node")){
    macStrToByteArray(incomingReadings.source,remote_wifi_next);
    Serial.println("nuovo nodo aggiunto");

    memcpy(peerInfo3.peer_addr, remote_wifi_next, 6);
    peerInfo3.channel = 1;  
    peerInfo3.encrypt = false;

    if(esp_now_add_peer(&peerInfo3)!= ESP_OK){
      Serial.println("Failed to add discovered peer");
      return;
    }
    BLE.stopAdvertise();
  }else if(String(incomingReadings.dest).equals(WiFi.macAddress())){
    Serial.println("arrivato a destinazione");
    Serial.println(incomingReadings.text);
  }
  else{
    Serial.println("inoltro al prossimo");

    char remote_wifi_prec_str[18];
    snprintf(remote_wifi_prec_str, sizeof(remote_wifi_prec_str), "%02x:%02x:%02x:%02x:%02x:%02x",
         remote_wifi_prec[0], remote_wifi_prec[1], remote_wifi_prec[2],
          remote_wifi_prec[3], remote_wifi_prec[4], remote_wifi_prec[5]);


    char mac_str[18];
    snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
         mac[0], mac[1], mac[2],
          mac[3], mac[4], mac[5]);

    String mac_String(mac_str);
    
    if(mac_String.equals(remote_wifi_prec_str)){
      Serial.println("inoltro a next");
      int retry=0;
      while(!send_data_to(remote_wifi_next)){
        retry++;
        if(retry==10)   
          break;
      }

    }
    else{
      if(i_m_gateway){
        Serial.println("invio lora");
        //invia con lora 
        BLE.

      }
      Serial.println("inoltro a prec");
      int retry=0;
      while(!send_data_to(remote_wifi_prec)){
        retry++;
        if(retry==10)   
          break;
      }
    }

  }

}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.println();

  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }
  BLE.setLocalName("Ancora");

  BLE.advertise();
  if(!i_m_gateway)
    BLE.scanForName("Ancora");
  
  while(!i_m_gateway){
    BLEDevice peripheral = BLE.available();
    if(peripheral){
      Serial.println("Address: " + peripheral.address());
      BLE.stopScan();

      macStrToByteArray(peripheral.address(),remote_ble);
      macStrToByteArray(peripheral.address(),remote_wifi_prec);
      remote_wifi_prec[5]=remote_wifi_prec[5]-2;
      for(int i = 0; i < 6; i++){
        Serial.print(remote_ble[i],HEX);
        Serial.print(":");
      }
      Serial.println();
      
      peripheral.disconnect();
      
      BLE.stopScan();
      break;
    }

    Serial.println("searching");
    delay(500);
  }


  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer broadcast
  for(int i = 0; i < 6; i++){
    Serial.print(broadcastAddress[i],HEX);
    Serial.print(":");
  }
  Serial.println();

  memcpy(peerInfo2.peer_addr, broadcastAddress, 6);
  peerInfo2.channel = 1;  
  peerInfo2.encrypt = false;
  if(esp_now_add_peer(&peerInfo2)!= ESP_OK){
    Serial.println("Failed to add broadcastAddress peer");
    return;
  }

  if(!i_m_gateway){

    for(int i = 0; i < 6; i++){
      Serial.print(remote_wifi_prec[i],HEX);
      Serial.print(":");
    }
    Serial.println();

    // Register peer wifi
    memcpy(peerInfo.peer_addr, remote_wifi_prec, 6);
    peerInfo.channel = 1;  
    peerInfo.encrypt = false;
    // Add peer
    if(esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Failed to add peer");
      return;
    }
    
  strcpy(message.type,String("new node").c_str());
  message.dest[String("new node").length()]='\0';

  strcpy(message.source,WiFi.macAddress().c_str());
  message.dest[WiFi.macAddress().length()]='\0';

  strcpy(message.text,String("").c_str());
  message.dest[String("").length()]='\0';


    char mac_str[18];
    snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
          remote_wifi_prec[0], remote_wifi_prec[1], remote_wifi_prec[2],
          remote_wifi_prec[3], remote_wifi_prec[4], remote_wifi_prec[5]);

    strcpy(message.dest,String(mac_str).c_str());
    message.dest[String(mac_str).length()]='\0';

    int retry=0;
    while(!send_data_to(remote_wifi_prec)){ 
      retry++;
        if(retry==10)   
          break;
      }

    
  }
  
  ReadBLE.disable();
  //SendBLE.disable();

  if(!i_m_gateway){
    ts.addTask(ReadBLE);
    //ts.addTask(SendBLE);
    ReadBLE.enable();
    //SendBLE.enable();
  }
  
  
  ts.startNow();
}

void loop() {

  if(false){
    delay(5000);
    
    strcpy(message.type,String("mess").c_str());
    strcpy(message.source,WiFi.macAddress().c_str());
    strcpy(message.text,String("ciao").c_str());
    
    char mac_str[18];
    snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
          broadcastAddress[0], broadcastAddress[1], broadcastAddress[2],
          broadcastAddress[3], broadcastAddress[4], broadcastAddress[5]);

    strcpy(message.dest,String(mac_str).c_str());

    Serial.print("destinatario nuovo messagio:   ");
    Serial.println(message.dest);
    int retry=0;
    while(!send_data_to(remote_wifi_prec)){ 
      retry++;
        if(retry==10)   
          break;
      }
    
  }
  
  ts.execute();
}