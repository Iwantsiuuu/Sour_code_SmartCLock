/*
 * task_sensor.c
 *
 *  Created on: 16 Feb 2024
 *      Author: DwiSetyabudi
 *      Edited: Ridwan Bahari (21 Mei 2024)
 */

#include "task_sensor.h"

#include "main.h"
#include "bme68x.h"

sensor_data dps_sensor;
sensor_data bmp280_sensor;
sensor_data bme680_sensor;

/*Sensor parameter & variables*/
struct bme68x_dev bme_device;
struct bme68x_conf bme_config;
struct bme68x_data bme68x_data;
struct bme68x_heatr_conf bme_heater_config;
uint32_t bme_period;

xensiv_dps3xx_t pressure_sensor;

void sensor_Reading(void *arg)
{
	//wait for systems to be ready to ensure all systems start up correctly
	while(!systemReady)
	{
		vTaskDelay(pdMS_TO_TICKS(5));
	}

#ifdef USE_SENSOR
	uint32_t time_update = 0;
#endif

	while(1)
	{
#ifdef USE_DUMMY_DATA
		dps_sensor.pressure = 1014.25;
		dps_sensor.temperature = 28;

		bmp280_sensor.pressure = 1014.25;
		bmp280_sensor.temperature = 28;

		bme680_sensor.pressure = 1014.25;
		bme680_sensor.temperature = 28;
		bme680_sensor.humidity = 60;
		bme680_sensor.gas = 50;
#endif

#ifdef USE_SENSOR
		if( xTaskGetTickCount() - time_update >= 1000)
		{
			if (semphr_i2c_dev != NULL)
			{
				if(xSemaphoreTake(semphr_i2c_dev, ( TickType_t ) 100))
				{
#ifdef USE_DPS310
					xensiv_dps3xx_read(&pressure_sensor, &dps_sensor.pressure, &dps_sensor.temperature);
					//					printf("press: %0.2f \t tempt: %0.0f C\r\n",dps_sensor.pressure, dps_sensor.temperature);
#endif

#ifdef USE_BMP280
					BMP280_readValue(&bmp280_sensor.temperature, &bmp280_sensor.pressure, 100);
					//					printf("press: %0.2f \t tempt: %0.0f C\r\n",dps_sensor.pressure, dps_sensor.temperature);
#endif

#ifdef USE_BME680
					/* Read compensated data */
					bme68x_get_data(BME68X_FORCED_MODE, &bme68x_data, &n_data_ptr, &bme_device);
					bme680_sensor.pressure = bme68x_data.pressure;
					bme680_sensor.temperature = bme68x_data.temperature;
					bme680_sensor.humidity = bme68x_data.humidity;
					bme680_sensor.gas = bme68x_data.gas_resistance;
#endif
					xSemaphoreGive(semphr_i2c_dev);
					time_update = xTaskGetTickCount();
				}
			}

		}
#endif
		vTaskDelay(100);
	};
}

//Initialize sensor for environment and air quality data
cy_rslt_t sensorInit()
{
	cy_rslt_t result;
#ifdef USE_BMP280
	result = bmp280.init(&i2c, BMP280_ADDR);
	return result;
#endif

#ifdef USE_DPS310
	result = xensiv_dps3xx_mtb_init_i2c(&pressure_sensor, &i2c, 0x77);
	return result;
#endif

#ifdef USE_BME680
	init_bme68x_sensor();
    uint8_t n_data_ptr;
#endif

#ifdef USE_DUMMY_DATA
	result = CY_RSLT_SUCCESS;
	return result;
#endif
}

BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
	uint8_t dev_addr = *(uint8_t*)intf_ptr;

    cy_rslt_t result = cyhal_i2c_master_write(&i2c, BME68X_I2C_ADDR_LOW, &reg_addr, 1, 10, false);

    	if (CY_RSLT_SUCCESS == result)
    	{
    		result = cyhal_i2c_master_read(&i2c, BME68X_I2C_ADDR_LOW, reg_data, length, 10, true);
    	}

    return result;
}

/*!
 * I2C write function map to COINES platform
 */
BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
	uint8_t dev_addr = *(uint8_t*)intf_ptr;

    uint8_t buf[32];
	buf[0] = reg_addr;
	for (uint16_t i=0; i < length; i++)
	{
		buf[i+1] = reg_data[i];
	}

	cy_rslt_t result = cyhal_i2c_master_write(&i2c, BME68X_I2C_ADDR_LOW, buf, length+1, 10, true);

    return result;
}

void bme68x_delay_us(uint32_t period, void *intf_ptr)
{
	vTaskDelay(period);
}

void init_bme68x_sensor()
{
	bme_device.write = bme68x_i2c_write;
	bme_device.read = bme68x_i2c_read;
	bme_device.delay_us = bme68x_delay_us;

		/* set correct i2c address */
	//bme_device.chip_id = BME280_SHUTTLE_ID;
	bme_device.intf = BME68X_I2C_INTF;

	bme68x_init(&bme_device);

	/* Always read the current settings before writing, especially when all the configuration is not modified */
	bme68x_get_conf(&bme_config, &bme_device);

	/* Configuring the over-sampling rate, filter coefficient and standby time */
	/* Overwrite the desired settings */
	bme_config.filter = BME68X_FILTER_SIZE_3;

	/* Over-sampling rate for humidity, temperature and pressure */
	bme_config.os_hum = BME68X_OS_2X;
	bme_config.os_pres = BME68X_OS_4X;
	bme_config.os_temp = BME68X_OS_8X;

	bme_heater_config.enable = BME68X_ENABLE;
	bme_heater_config.heatr_temp = 320;
	bme_heater_config.heatr_dur = 150;

	/* Setting the standby time */
	bme_config.odr = BME68X_ODR_NONE;

	bme68x_set_conf(&bme_config, &bme_device);

	/* Calculate measurement time in microseconds */
	bme68x_get_meas_dur(BME68X_FORCED_MODE,&bme_config, &bme_device);

	bme68x_set_heatr_conf(BME68X_FORCED_MODE, &bme_heater_config, &bme_device);

	/* Always set the power mode after setting the configuration */
	bme68x_set_op_mode(BME68X_FORCED_MODE, &bme_device);
}
