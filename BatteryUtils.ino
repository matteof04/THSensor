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
uint8_t getBatteryPercentage(){
  int val = analogRead(BATTERY_PIN);
  float perc = map(val, 0, 1023, 0, 500);
  float res = (BATTERY_R1 + BATTERY_R2) / BATTERY_R2;
  float voltage = perc * res / 100;
  float singleCellVoltage = voltage/6;
  if (singleCellVoltage < 1.1) {
    return 0;
  }
  if (singleCellVoltage > 1.4) {
    return 100;
  }
  uint8_t percentage = (uint8_t)mapFloat(singleCellVoltage, 1.1, 1.4, 1, 99);
  return percentage;
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}