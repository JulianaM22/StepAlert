#include <TinyGPSPlus.h> //I want to read gps module data from serial and send it to usb serial and print it
TinyGPSPlus gps; //creating function to interface 
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);// TAAKING OUT FOR TEST
USBSerial.begin(9600);
delay(3000);

}

void loop() {
  USBSerial.println("TEST1");
  // put your main code here, to run repeatedly:
  USBSerial.write(Serial.read());//TAKING OUT FOR TEST
  gps.encode(USBSerial.read());// TAKING OUT FOR TEST
  USBSerial.println(gps.speed.mph());
  USBSerial.println(" is speed\n");
  USBSerial.println("TEST2");
  delay(3000); 
  //USBSerial.println("TESTING");
}
