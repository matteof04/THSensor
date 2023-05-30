/* Copyright (C) 2023 Matteo Franceschini <matteof5730@gmail.com>
*
*  This file is part of THSensor.
*  SmartBase is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
*  THSensor is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*  See the GNU Affero General Public License for more details.
*
*  You should have received a copy of the GNU Affero General Public License along with THSensor.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <Timer.h>
#include <avr/pgmspace.h>
#include "printf.h"
#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <EEPROM.h>
#include <DHT.h>

#include "Config.h"
#include "uuid.h"
#include "DeviceId.h"

void(* reboot)(void) = 0;

RF24 radio(CE_PIN, CS_PIN);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

DHT dht(DHT_PIN, DHT_TYPE);

int buttonState = 0;     // current state of the button
int lastButtonState = 0; // previous state of the button
int startPressed = 0;    // the moment the button was pressed
int endPressed = 0;      // the moment the button was released
int holdTime = 0;        // how long the button was hold

Timer checkConnectionTimer = Timer();

void setup() {
  pinMode(RESET_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  printf_begin();
  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (true) {}
  }
  byte channel = EEPROM.read(CHANNEL_EEPROM_LOCATION);
  if (channel == 0) {
    for (int i = 95; i <= 125; i++) {
      bool begin = mesh.begin(i);
      bool renew = mesh.renewAddress(1000) != MESH_DEFAULT_ADDRESS;
      Serial.println("Channel "+String(i)+" begin: "+String(begin)+" and renew "+String(renew));
      if(begin && renew){
        delayMicroseconds(150);
        unsigned long start = micros();
        bool write = mesh.write(0, PING_PACKET, 0);
        unsigned long stop = micros();
        unsigned long elapsed_time = stop-start;
        Serial.println("Channel "+String(i)+" write:"+String(write));
        if (write) {
          Serial.println("Channel"+String(i)+": Success with time "+String(elapsed_time));
        }
      }
    }
  }
  /*while (true) {}*/
  byte nodeID = EEPROM.read(NODE_ID_EEPROM_LOCATION);
  mesh.setNodeID(nodeID == 0? 255 : nodeID);
  mesh.begin(channel);
  dht.begin();
  radio.printPrettyDetails();
  if(mesh.getNodeID() == 255){
    Serial.println("Need a new NODE ID!");
    mesh.write(0, NODE_ID_REQUEST_PACKET, 0);
    while(true){
      mesh.update();
      while (network.available()) {
        RF24NetworkHeader header;
        network.peek(header);
        switch (header.type) {
          case NODE_ID_ASSIGNMENT_PACKET:
            handle_node_id_assignment(header);
            if(mesh.releaseAddress()){
              reboot();
            }
            break;
          default:
            Serial.print(F("*** WARNING *** Unknown message type "));
            Serial.println(header.type);
            handle_error(header);
            break;
        }
      }
    }
  }
  Serial.println("No need for a new node ID");
  checkConnectionTimer.every(30000, checkConnection, true);
}

void loop() {
  buttonState = digitalRead(RESET_PIN);
  if (buttonState != lastButtonState) {
     updateState();
  }
  lastButtonState = buttonState;
  checkConnectionTimer.update();
  mesh.update();
  while (network.available()) {
    Serial.println("Packet received");
    RF24NetworkHeader header;
    network.peek(header);
    Serial.print("Packet type: ");
    Serial.println(header.type);
    switch (header.type) {
      case NODE_ID_ASSIGNMENT_PACKET:
        handle_node_id_assignment(header);
        reboot();
      case INFO_REQUEST_PACKET:
        handle_info_request(header);
        break;
      case DATA_REQUEST_PACKET:
        Serial.println("Data request packet received. Sending response...");
        handle_data_request(header);
        break;
      default:
        Serial.print(F("*** WARNING *** Unknown message type "));
        Serial.println(header.type);
        handle_error(header);
        break;
    }
  }
  while(Serial.available()){
    byte channel = Serial.parseInt();
    EEPROM.write(CHANNEL_EEPROM_LOCATION, channel);
    reboot();
  }
}

void updateState() {
  if (buttonState == LOW) {
    Serial.println("Button pressed");
    startPressed = millis();
  } else {
    endPressed = millis();
    holdTime = endPressed - startPressed;
      if (holdTime >= 1000 && holdTime < 3000) {
        Serial.println("Confirm Assoc");
        mesh.write(0, BTN_CONFIRM_PACKET, 0);
      }
      if (holdTime >= 10000) {
        Serial.println("Reset Assoc");
        mesh.write(0, BTN_RESET_PACKET, 0);
      }

  }
}
