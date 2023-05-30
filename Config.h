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
#ifndef CONFIG_H
#define CONFIG_H
//PACKETS
#define NODE_ID_REQUEST_PACKET 0
#define NODE_ID_ASSIGNMENT_PACKET 65
#define ERROR_PACKET 66
#define INFO_REQUEST_PACKET 1
#define INFO_PACKET 67
#define DATA_REQUEST_PACKET 2
#define TH_SENSOR_DATA_PACKET 68
#define PLANT_SENSOR_DATA_PACKET 69
#define REBOOT_PACKET 70
#define BTN_CONFIRM_PACKET 71
#define BTN_RESET_PACKET 72
#define PING_PACKET 73
//RADIO
#define CE_PIN 7
#define CS_PIN 8
#define NODE_ID_EEPROM_LOCATION 0
#define CHANNEL_EEPROM_LOCATION 1
//DHT
#define DHT_PIN 3
#define DHT_TYPE DHT11
//BATTERY
#define BATTERY_PIN A0
#define BATTERY_R1 10000
#define BATTERY_R2 10000
//RESET
#define RESET_PIN 2
#endif