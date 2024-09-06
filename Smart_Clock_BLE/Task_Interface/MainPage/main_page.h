#ifndef MAIN_PAGE_H_
#define MAIN_PAGE_H_

#include "interface.h"

void main_page();
static void init_main_page();
static void deinit_main_page();
static void main_page_draw();

extern struct tm RTC_TIME;

#endif
