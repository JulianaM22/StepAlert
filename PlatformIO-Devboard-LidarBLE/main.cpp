#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>  //esp32 ble library
#include <BLEServer.h>
#include "LIDARLite_v4LED.h"

LIDARLite_v4LED myLIDAR;  //Click here to get the library: http://librarymanager/All#SparkFun_LIDARLitev4 by SparkFun
float newDistance;

const int READPIN = A0;                                             //define pin here??
const char *SERVICE_UUID = "743b9a27-2e82-48eb-8254-04c8ac77a29d";  //how come this many bytes?
const char *VALUE_UUID = "d7daffe4-ae89-44e2-9c9f-66fc2b13573c";
BLEServer *pServer = NULL;
BLECharacteristic *data_charic = NULL;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    Serial.println("Client connected.");  //someone connected to our serveice when this prints, since onConnect will trigger this to print
  }
  void onDisconnect(BLEServer *pServer) {
    Serial.println("Client disconnected.");
    BLEDevice::startAdvertising();  //to make sure our client sees the device even if we are not connected
  }
};
class Data_charic_Callback : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic) {
    //not sure what to put here???!!! we want the user to be able to read this
    //int sensorValue = analogRead(READPIN);//define pin???
    //Serial.println(sensorValue);
    int newNewDistance = (int)newDistance;
    pCharacteristic->setValue(newNewDistance);
  }
};

void setup() {
  USBSerial.begin(9600);
  USBSerial.println("Qwiic LIDARLite_v4 examples");
  Wire.begin(40, 41);  //Join I2C bus

  //check if LIDAR will acknowledge over I2C
  if (myLIDAR.begin() == false) {
    USBSerial.println("Device did not acknowledge! Freezing.");
    while (1)
      ;
  }
  USBSerial.println("LIDAR acknowledged!");

  uint8_t acq_count = 0x15;
  uint8_t acq_count_reg = 0x05;

  myLIDAR.write(acq_count_reg, &acq_count, 1);

  pinMode(READPIN, OUTPUT);             //sets up connection between the potentiometer on the board and the program
  BLEDevice::init("NUMBER TESTER");     //namespace with a freestanding function called emit
  pServer = BLEDevice::createServer();  //pointer because its an address t


  BLEService *pService = pServer->createService(SERVICE_UUID);
  data_charic = pService->createCharacteristic(
    VALUE_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);

  pServer->setCallbacks(new ServerCallbacks());           //kind of declaring a function
  data_charic->setCallbacks(new Data_charic_Callback());  //setting callbacks so we can access that part of the object to read it
                                                          //the callbacks tells server what to do when specific events occur,
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  //We don't know if these are correct for PC bluetooth
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();


}  //created a characteristic for us to read when we connect on the phone, it is public so anyone


void loop() {

  //getDistance() returns the distance reading in cm
  newDistance = myLIDAR.getDistance();
  // notify changed value

  int uintDistance = (int)newDistance;
  data_charic->setValue(uintDistance);
  data_charic->notify();
  //delay(3);  // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms


  //Print to Serial port
  USBSerial.print("New distance:");
  USBSerial.print(newDistance / 100);
  USBSerial.println(" m");

  //delay(2);  //Don't hammer too hard on the I2C bus
}
