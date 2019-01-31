#include <gtk/gtk.h>
#include <stdlib.h>
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "util.h"
#include "area1.h"
#include "menus.h"
#include "gui.h"


//Initialisation de la fenetre

void on_app_activate (GtkApplication* app, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
		
	g_object_set (gtk_settings_get_default(),"gtk-shell-shows-menubar", FALSE, NULL);

	window_init(app,my);

	menus_init(my);

	area1_init(my);

	status_init(my);
	
	editing_init(my);
	
	layout_init(my);

	win_scale_init(my);

	gtk_widget_show_all (my->window);
	
	gtk_widget_add_events (my->area1,GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK | GDK_FOCUS_CHANGE_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);
	
	gtk_widget_set_can_focus (my->area1, TRUE);
	
	gtk_widget_hide(my->frame1);
}

int main (int argc, char *argv[])
{
	GtkApplication *app;
	int status;
	Mydata my;
	init_my_data(&my);
	app = gtk_application_new (NULL, G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate",G_CALLBACK(on_app_activate), &my);
	status = g_application_run (G_APPLICATION(app), argc, argv);
	g_object_unref (app);
	return status;
}

