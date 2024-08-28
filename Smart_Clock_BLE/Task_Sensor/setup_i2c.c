#include "setup_i2c.h"

#ifdef USE_SMART_CLOCK_DEVELOPMENT_BOARD
#define SDA_PIN (P10_1)
#define SCL_PIN	(P10_0)
#endif

/*I2C object*/
cyhal_i2c_t i2c;

/*-------- Configure I2C as master and set the frequency to fast(400 Khz) ----------*/
const cyhal_i2c_cfg_t i2c_cfg =
{
		.is_slave 		 = false,
		.address  		 = 0,
		.frequencyhal_hz = 400000
};

cy_rslt_t init_i2c()
{

	cy_rslt_t result;

#ifdef USE_EXPANSION_BOARD
	/*-------- I2C initialization when using the CY8CKIT-028-SENSE shield in the board support package ------------*/
	result = cyhal_i2c_init(&i2c, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
#endif

#ifdef USE_SMART_CLOCK_DEVELOPMENT_BOARD
	/*-------- I2C initialization when using the smart clock board development ------------*/
	result = cyhal_i2c_init(&i2c, SDA_PIN, SCL_PIN, NULL);
#endif

	/*I2C pin for development board Smart Clock (CYBLE-416045-02)*/
	//	result = cyhal_i2c_init(&i2c, SDA_PIN, SCL_PIN, NULL);
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	result = cyhal_i2c_configure(&i2c, &i2c_cfg);
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	return result;
}
