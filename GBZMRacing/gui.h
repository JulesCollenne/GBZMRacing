#ifndef GUI_H
#define GUI_H

void editing_init(Mydata *my);
void status_init(gpointer user_data);
void set_edit_mode(Mydata *my,int mode);
void layout_init(gpointer user_data);
void window_init(GtkApplication* app, gpointer user_data);
void win_scale_init (gpointer user_data);

#endif
