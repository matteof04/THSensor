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
struct InfoPacket {
  byte sensorType;
  uuid_t deviceId;
};

struct DataPacket {
  float temperature;
  float humidity;
  float hic;
  uint8_t batteryPercentage;
};

void handle_node_id_assignment(RF24NetworkHeader& receivedHeader){
  uint8_t nodeID = 0;
  network.read(receivedHeader, &nodeID, sizeof(nodeID));
  Serial.print("New nodeID: ");
  Serial.println(nodeID, BIN);
  EEPROM.write(NODE_ID_EEPROM_LOCATION, nodeID);
}

void handle_info_request(RF24NetworkHeader& receivedHeader){
  /*
  0 --> Base
  1 --> Sonda TH
  2 --> Sonda SmartPlant
  */
  network.read(receivedHeader, 0, 0);
  byte sensorType = 1;
  uuid_t deviceId = DEVICE_ID;
  InfoPacket infoPacket;
  infoPacket.sensorType = sensorType;
  uuid_copy(infoPacket.deviceId, deviceId);
  mesh.write(&infoPacket, INFO_PACKET, sizeof(infoPacket));
}

void handle_data_request(RF24NetworkHeader& receivedHeader){
  network.read(receivedHeader, 0, 0);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float hic = dht.computeHeatIndex(temperature, humidity, false);
  Serial.print("Temp: ");
  Serial.println(temperature);
  Serial.print("Hum: ");
  Serial.println(humidity);
  Serial.print("HIC: ");
  Serial.println(hic);
  uint8_t batteryPercentage = getBatteryPercentage();
  DataPacket dataPacket = DataPacket { temperature, humidity, hic, batteryPercentage };
  mesh.write(&dataPacket, TH_SENSOR_DATA_PACKET, sizeof(dataPacket));
}

void handle_reboot(RF24NetworkHeader& receivedHeader){
  network.read(receivedHeader, 0, 0);
  reboot();
}

void handle_error(RF24NetworkHeader& receivedHeader){
  network.read(receivedHeader, 0, 0);
  mesh.write(0, ERROR_PACKET, 0);
}

void checkConnection(){
  if (!mesh.checkConnection()) {
    Serial.println("Connection failed, retrieving a new address...");
    if(mesh.renewAddress() == MESH_DEFAULT_ADDRESS){
      Serial.println("Reconnect to the network...");
      byte channel = EEPROM.read(CHANNEL_EEPROM_LOCATION);
      mesh.begin(channel);
      Serial.println("Finished");
    }
  }
}
