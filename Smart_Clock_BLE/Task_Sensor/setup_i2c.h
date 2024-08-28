#pragma once

#ifndef SETUP_I2C_H_
#define SETUP_I2C_H_

#include "main.h"

extern cyhal_i2c_t i2c;

cy_rslt_t init_i2c();

#endif
