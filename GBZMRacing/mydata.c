#include <gtk/gtk.h>
#include <stdlib.h>
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "util.h"
#include "area1.h"
#include "menus.h"
#include "gui.h"

#define WIN_W 1000
#define WIN_H 600

void init_my_data(Mydata *my){
	my->win_width = WIN_W;
	my->win_height = WIN_H;
	my->title = "[GaBuZoMeu Racing]";
	my->magic=MYDATA_MAGIC;
	my->current_folder=NULL;
	my->pixbuf1=NULL;
	my->scale1_value = 1.0;
	my->pixbuf2=NULL;
	my->rotate_angle=0;
	my->click_x = 0;
	my->click_y = 0;
	my->click_n = 0;
	my->clip_image = FALSE;
	my->show_edit = FALSE;
	my->edit_mode = EDIT_ADD_CURVE;
	init_curve_infos(&my->curve_infos);
	my->bsp_mode = BSP_ROAD;
	init_game(&my->game);
	my->timeout1 = 0;
	int i;
	for(i=0;i<CAR_MAX;i++)
	{
		my->flag_key_left[i] = 0;
		my->flag_key_right[i] = 0;
		my->flag_key_up[i] = 0;
		my->flag_key_down[i] = 0;
	}
}

Mydata *get_mydata (gpointer data)
{
	if (data == NULL) {
		fprintf (stderr, "get_mydata: NULL data\n"); return NULL;
	}
	if (((Mydata *)data)->magic != MYDATA_MAGIC) {
		fprintf (stderr, "get_mydata: bad magic number\n"); return NULL;
	}
	return data;
}
