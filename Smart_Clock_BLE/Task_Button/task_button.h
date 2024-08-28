#pragma once

#ifndef TASK_BUTTON_H_
#define TASK_BUTTON_H_

#include "button.h"

#define BTN_ENTER 	(P6_2)
#define BTN_BACK	(P6_5)
#define BTN_UP		(P6_4)
#define BTN_DOWN	(P5_4)

typedef enum buttonCode {
	BUTTON_UP 		= 0,
	BUTTON_DOWN 	= 1,
	BUTTON_ENTER 	= 2,
	BUTTON_BACK 	= 3
}BTN_CODE;

//extern bool systemReady;

void ButtonApp();
void ButtonInit();

extern button_var btn_obj[NUM_OF_BTN];

#endif
