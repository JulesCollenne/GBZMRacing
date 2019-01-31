#ifndef MYDATA_H
#define MYDATA_H

#define MYDATA_MAGIC 0x46EA7E05

enum {EDIT_NONE,EDIT_ADD_CURVE,EDIT_MOVE_CURVE,EDIT_REMOVE_CURVE,EDIT_ADD_CONTROL,EDIT_MOVE_CONTROL,EDIT_REMOVE_CONTROL,EDIT_LAST};

enum {BSP_FIRST,BSP_ROAD,BSP_TRACK,BSP_LAST};

typedef struct {
	GtkWidget *window;
	char *title;
	int win_width;
	int win_height;
	unsigned int magic;
	char *current_folder;
	GtkWidget *status, *vbox1, *win_scale, *scale1, *area1, *menu_bar, *scroll, *frame1, *hbox1, *edit_radios[EDIT_LAST];
	GdkPixbuf *pixbuf1;
	GtkWidget *item_editing;
	gdouble scale1_value;
	GdkPixbuf  *pixbuf2;
	double rotate_angle;
	double click_x;
	double click_y;
	int click_n;
	int clip_image;
	int show_edit;
	int edit_mode;
	int bsp_mode;
	GtkWidget *bsp_radios[BSP_LAST];
	Curve_infos curve_infos;
	double last_x, last_y;
	Game game;
	guint timeout1;
	int flag_key_left[CAR_MAX], flag_key_right[CAR_MAX], flag_key_up[CAR_MAX], flag_key_down[CAR_MAX];
} Mydata;

void init_my_data(Mydata *my);

Mydata *get_mydata (gpointer data);

#endif
