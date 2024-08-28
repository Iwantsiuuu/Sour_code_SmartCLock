#ifndef MENU_H_
#define MENU_H_

#include "interface.h"

#ifndef INTERFACE_MAXCHAR
#define INTERFACE_MAXCHAR 22
#endif

typedef struct interface {
	u8g2_t *u8g2_obj;
	uint8_t total_menu;
	uint8_t position;
	uint8_t offset;

	const char **menu;
	unsigned char set_title;
	char title [INTERFACE_MAXCHAR];

	uint8_t cursor;
}menu_t;

void interface_construct(menu_t *menu_, u8g2_t *menu_obj);
void interface_begin(menu_t *menu_);
void interface_next(menu_t *menu_);
void interface_previous(menu_t *menu_);
void interface_reset_position(menu_t *menu_);

void interface_set_menu(menu_t *menu_, const char **menu, uint8_t total_menu);

uint8_t interface_getPosition(menu_t *menu_);

void interface_draw_menu(menu_t *menu_);
void interface_draw(menu_t *menu_);
void interface_clearAll(menu_t *menu_);

void interface_cursor(menu_t *menu_, uint8_t cursor);
void interface_setTitle(menu_t *menu_, const char *title);


#endif
