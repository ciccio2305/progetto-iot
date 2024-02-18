
#include <BLEDevice.h>
#include <SPI.h>     
#include <Arduino.h>

BLEClient*  pClient;
BLEAdvertisedDevice myAncora;
bool AncoraFound = false;
BLEScan *pBLEScan;


class callbackgenerica: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
   //printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
  }
};

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914c"
#define ID_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a7"

String scan_ancore_blocked(){
    String ID;
    while (true)
    {
        Serial.println("Reading BLE");
        BLEScan* pBLEScanBraccialetto = BLEDevice::getScan();
        pBLEScanBraccialetto->setAdvertisedDeviceCallbacks(new callbackgenerica());
        BLEScanResults foundDevices = pBLEScanBraccialetto->start(5);

        for(int i = 0; i < foundDevices.getCount(); i++){
            BLEAdvertisedDevice peripheral=foundDevices.getDevice(i);

            if(peripheral.haveName()&&peripheral.getName()=="Ancora"){

                pClient->connect(peripheral.getAddress());

                BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
                if(pRemoteService==nullptr){
                    Serial.println("service not found");
                    continue;
                }

                BLERemoteCharacteristic* pRemoteCharacteristic=  pRemoteService->getCharacteristic(ID_CHARACTERISTIC_UUID);
                if(pRemoteCharacteristic==nullptr){
                    Serial.println("characteristic not found");
                    continue;
                }

                String ID(pRemoteCharacteristic->readValue().c_str());

                pClient->disconnect();
                break;
            }   
        }
    }
    
    return ID;
}