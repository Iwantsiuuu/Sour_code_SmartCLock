/*
 * task_sensor.h
 *
 *  Created on: 16 Feb 2024
 *      Author: DwiSetyabudi
 *      Edited: RidwanBahari (21 Mei 2024)
 */
#pragma once

#ifndef TASK_SENSOR_TASK_SENSOR_H_
#define TASK_SENSOR_TASK_SENSOR_H_

#include "BMP280.h"
#include "bme68x.h"

typedef struct{
	float temperature,
		  pressure,
		  humidity,
		  gas;
}sensor_data;

extern sensor_data dps_sensor;
extern sensor_data bmp_sensor;
extern sensor_data bme680_sensor;

void sensor_App();
cy_rslt_t sensorInit();

BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);
void bme68x_delay_us(uint32_t period, void *intf_ptr);
void init_bme68x_sensor();

#endif /* TASK_SENSOR_TASK_SENSOR_H_ */
