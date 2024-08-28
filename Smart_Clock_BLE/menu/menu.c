#include "menu.h"

#include <string.h>

// Function success
void interface_construct(menu_t *menu_, u8g2_t *menu_obj)
{
	menu_->u8g2_obj = menu_obj;
	menu_->menu = NULL;
	menu_->total_menu = 0;
	menu_->position = 0;
	menu_->offset = 0;
	menu_->cursor = 1;
	u8g2_SetFont(menu_->u8g2_obj, u8g_font_profont11);
}

// Function success
void interface_begin(menu_t *menu_)
{
	menu_->position = 0;
	/* Initialize the U8 Display */
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(menu_->u8g2_obj, U8G2_R0, u8x8_byte_hw_i2c,
			u8x8_gpio_and_delay_cb);
	/* Send init sequence to the display, display is in sleep mode after this */
	u8g2_InitDisplay(menu_->u8g2_obj);
	/* Wake up display */
	u8g2_SetPowerSave(menu_->u8g2_obj, 0);

	u8g2_SetFont(menu_->u8g2_obj, u8g_font_profont11);

	u8g2_ClearDisplay(menu_->u8g2_obj);
	u8g2_ClearBuffer(menu_->u8g2_obj);
}

void interface_draw_menu(menu_t *menu_)
{
	uint8_t x = 0, y = 10;
	unsigned char total_menu_show = menu_->menu < 5 ? menu_->total_menu : 5;
	unsigned char menu_offset = menu_->position < 5 ? 0 : menu_->position - 4;

	if (((menu_->position < menu_->offset) || (menu_->position >= menu_->offset + 4)) && menu_->cursor > 0)
	{
		if (menu_->offset > menu_offset)
			menu_->offset--;
		else
			menu_->offset = menu_offset;
	}
	const char *menu_show[total_menu_show];

	if (menu_->set_title == 1)
	{
		u8g2_DrawStr(menu_->u8g2_obj, x, y-5, menu_->title);
		y+=10;
	}

	for (uint8_t i = 0; i < (menu_->set_title == 1 ? 4 : 5) && i < total_menu_show; i++)
	{
		menu_show[i] = menu_->menu[i + menu_->offset];
		if (menu_->cursor > 0)
		{
			if (menu_->position - menu_->offset == i)
			{
				u8g2_DrawStr(menu_->u8g2_obj, x+8, y+(10*i), ">");
				if (menu_show[i] != NULL)
					u8g2_DrawStr(menu_->u8g2_obj, x+16, y+(10*i), menu_show[i]);
			}
			else
			{
				if (menu_show[i] != NULL)
					u8g2_DrawStr(menu_->u8g2_obj, x+16, y+(10*i), menu_show[i]);
			}
		}
		else
		{
			if (menu_show[i] != NULL)
				u8g2_DrawStr(menu_->u8g2_obj, x+5, y+(10*i), menu_show[i]);
		}
	}
	u8g2_DrawTriangle(menu_->u8g2_obj, 118, 57, 126, 57, 122, 62);
	u8g2_DrawTriangle(menu_->u8g2_obj, 118, 20, 126, 20, 122, 15);
}

// Function success
void interface_next(menu_t *menu_)
{
	menu_->position++;
	menu_->position = menu_->position % menu_->total_menu;
}

// Function success
void interface_previous(menu_t *menu_)
{
	menu_->position += menu_->total_menu;
	menu_->position --;
	menu_->position = menu_->position % menu_->total_menu;
}

// Function success
void interface_reset_position(menu_t *menu_)
{
	menu_->position = 0;
}

// Function success
uint8_t interface_getPosition(menu_t *menu_)
{
	return menu_->position;
}

// Function Success
void interface_set_menu(menu_t *menu_, const char **menu, uint8_t total_menu)
{
	menu_->menu = menu;
	menu_->total_menu = total_menu;
}

void interface_draw(menu_t *menu_)
{
	//go to first page
	u8g2_FirstPage(menu_->u8g2_obj);
	do{
		interface_draw_menu(menu_);
	}while (u8g2_NextPage(menu_->u8g2_obj));
}

// Function success
void interface_clearAll(menu_t *menu_)
{
	u8g2_ClearDisplay(menu_->u8g2_obj);
	u8g2_ClearBuffer(menu_->u8g2_obj);
}

void interface_cursor(menu_t *menu_, uint8_t cursor)
{
	menu_->cursor = cursor;
}

// Function success
void interface_setTitle(menu_t *menu_, const char *title)
{
	menu_->set_title = 1;
	strcpy(menu_->title, title);
}
