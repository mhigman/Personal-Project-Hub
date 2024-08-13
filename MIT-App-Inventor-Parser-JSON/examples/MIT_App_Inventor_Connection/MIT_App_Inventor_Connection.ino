#include <ArduinoJson.h>
#include "AppInventor.h"

#define COUNT_ENTRIES(ARRAY)    (sizeof(ARRAY) / sizeof(ARRAY[0]))


// Define constants pertinent to Bluetooth Low Energy initialization.
char uuid[37] = "4a988ad8-1cc4-e7c1-c757-f1267dd021e8";
const char * name = "Arduino";

//AppInventor::BLEConnector communication_manager{uuid, name}; 

AppInventor::Parser commander{};


BLEService interpreterService("4a988ad8-1cc4-e7c1-c757-f1267dd021e8");

BLEStringCharacteristic packet1("4a988ad9-1cc4-e7c1-c757-f1267dd021e8", BLERead | BLEWrite, 500);
BLEStringCharacteristic packet2("4a988ada-1cc4-e7c1-c757-f1267dd021e8", BLERead | BLEWrite, 500);
BLEStringCharacteristic packet3("4a988adb-1cc4-e7c1-c757-f1267dd021e8", BLERead | BLEWrite, 500);
BLEStringCharacteristic packet4("4a988adc-1cc4-e7c1-c757-f1267dd021e8", BLERead, 500);

void setup() {
  //communication_manager.begin();
  Serial.begin(115200);

  //communication_manager.begin();

  delay(2000);

  if(!BLE.begin()) {
    Serial.println("BLE no worke");
    while(1);
  }

  BLE.setLocalName("Arduino Interpreter");

  BLE.setAdvertisedService(interpreterService);

  interpreterService.addCharacteristic(packet1);
  interpreterService.addCharacteristic(packet2);
  interpreterService.addCharacteristic(packet3);
  interpreterService.addCharacteristic(packet4);

  BLE.addService(interpreterService);


  Serial.println("Hello");

  BLE.advertise();

}
/** 
 * @todo Check if command buffer overflows.
 */
char command[3000];
int serial_brace_count = 0;
int write_byte = 0;

void loop() {
  // put your main code here, to run repeatedly:
  //communication_manager.pollAndFindCentral();


  //String command = communication_manager.readChunk();
  if(Serial.available() > 0){
    Serial.println("Reading");
    while(Serial.available() > 0 && write_byte < 2950){
      char next_character = (char)Serial.read();
      if(next_character == '\0' || next_character == '\n'){
        continue;
      }
      command[write_byte] = next_character;
      if(command[write_byte] == '{'){
        serial_brace_count++;
      } else if(command[write_byte] == '}'){
        serial_brace_count--;
      }
      write_byte++;
    }
    Serial.println(command);
    if(serial_brace_count == 0){
      write_byte = 0;
      commander.prepareCommands(command);
      for(int i = 0; i < 3000; i++){
            command[i] = 0;
      }
    }
  }


  BLEDevice central = BLE.central();


  if(central) {

    int command_index = 0;
    int brace_count = 0;
    while(central.connected()) {
      
      bool packet1_write = packet1.written();
      bool packet2_write = packet2.written();
      bool packet3_write = packet3.written();
     
      if(packet1_write || packet2_write || packet3_write){
        for(char letter: packet1.value()) {
          if(letter != '\0'){
            if(letter == '{'){
              brace_count++;
            } else if(letter == '}'){
              brace_count--;
            }
            if(command_index >= 3000){
              for(int i = 0; i < 3000; i++){
                  command[i] = 0;
              }
              continue;
            }
            command[command_index] = letter;
            command_index++;
          }
        }
        //packet1.writeValue("");
        for(char letter: packet2.value()) {
          if(letter != '\0'){
            if(letter == '{'){
              brace_count++;
            } else if(letter == '}'){
              brace_count--;
            }
            if(command_index >= 3000){
              for(int i = 0; i < 3000; i++){
                  command[i] = 0;
              }
              continue;
            }
            command[command_index] = letter;
            command_index++;
          }
        }
        //packet2.writeValue("");
        for(char letter: packet3.value()) {
          if(letter != '\0'){
            if(letter == '{'){
              brace_count++;
            } else if(letter == '}'){
              brace_count--;
            }
            if(command_index >= 3000){
              for(int i = 0; i < 3000; i++){
                  command[i] = 0;
              }
              continue;
            }
            command[command_index] = letter;
            command_index++;
          }
        }
        //packet3.writeValue("");
        Serial.println(command);
        if(brace_count == 0){
          command_index = 0;
          commander.prepareCommands(command, &packet4);
          for(int i = 0; i < 3000; i++){
            command[i] = 0;
          }
        }
        packet1.setValue("");
        packet2.setValue("");
        packet3.setValue("");
        
      }
      delay(100);
    }
    
  }

  
  


  //communication_manager.readChunk(&command);
  //commander.prepareCommands(command);


  delay(1000);
}
