#ifndef MENUS_H
#define MENUS_H
void on_item_about_activate(GtkWidget *widget, gpointer data);
void on_item_rotate_activate(GtkWidget *widget, gpointer data);
void on_item_editing_activate(GtkWidget *widget, gpointer data);
void on_item_bg_color_activate (GtkWidget *widget, gpointer data);
void on_item_clip_activate(GtkWidget *widget, gpointer data);
void on_item_load_activate (GtkWidget *widget, gpointer data);
void on_item_scale_activate (GtkWidget *widget,gpointer data);
void on_item_quit_activate (GtkWidget *widget,gpointer data);
void menus_init(gpointer user_data);
void on_item_loadmap_activate (GtkWidget *widget,gpointer data);
void on_item_savemap_activate (GtkWidget *widget,gpointer data);

#endif
