#include <gtk/gtk.h>
#include <stdlib.h>
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "util.h"
#include "area1.h"
#include "menus.h"
#include "gui.h"

void on_scale1_value_changed(GtkWidget *widget, gpointer data)
{
    Mydata *my = data;
    if(my->pixbuf1 == NULL)
        return;

    if(my->scale1_value == gtk_range_get_value(GTK_RANGE(widget)))
        return;
        
	my->scale1_value = gtk_range_get_value(GTK_RANGE(widget));
	update_area1_with_transforms(my);
}

void win_scale_init (gpointer user_data){
	Mydata *my = get_mydata(user_data);
	GtkWidget *hbox1, *label;
	GtkAdjustment *ajustement;

	my->scale1_value = 1.0;

	my->win_scale = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(my->win_scale),"Image scale");
	gtk_widget_show_all(my->win_scale);
	g_signal_connect (my->win_scale, "delete-event",G_CALLBACK(gtk_widget_hide_on_delete), my);
	gtk_window_set_default_size(GTK_WINDOW(my->win_scale),300,100);

	hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_container_add (GTK_CONTAINER(my->win_scale), hbox1);

	label = gtk_label_new("Scale :");
	gtk_box_pack_start(GTK_BOX (hbox1),label, TRUE, TRUE, 10);
	ajustement = gtk_adjustment_new(1.0,0.02,20.0,0.2,1.0,1.0);
	my->scale1 = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,GTK_ADJUSTMENT(ajustement));
	gtk_box_pack_start (GTK_BOX (hbox1),GTK_WIDGET(my->scale1), TRUE, TRUE, 10);

	gtk_range_set_value(GTK_RANGE(my->scale1),1.0);
	
	g_signal_connect (my->scale1, "value-changed",G_CALLBACK(on_scale1_value_changed), my);

	my->scale1_value = gtk_range_get_value(GTK_RANGE(my->scale1));
	
	g_signal_connect (my->scale1, "value-changed",G_CALLBACK(on_scale1_value_changed), my);
	gtk_widget_hide(my->win_scale);
}


void window_init(GtkApplication* app, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
	my->window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (my->window), my->title);
	gtk_window_set_default_size (GTK_WINDOW (my->window),my->win_width,my->win_height);
}

void layout_init(gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
	my->vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL,4);
	gtk_container_add (GTK_CONTAINER(my->window), my->vbox1);
	
	my->scroll = gtk_scrolled_window_new(NULL,NULL);
	my->hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
	
	gtk_box_pack_start(GTK_BOX(my->vbox1),my->menu_bar,FALSE,FALSE,0);
	
	gtk_box_pack_start (GTK_BOX (my->vbox1), my->hbox1, TRUE, TRUE, 0);
	gtk_container_add (GTK_CONTAINER(my->scroll), my->area1);
	gtk_box_pack_start (GTK_BOX (my->vbox1), my->status, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (my->hbox1), my->frame1, FALSE, FALSE, 2);
	gtk_box_pack_start (GTK_BOX (my->hbox1), my->scroll, TRUE, TRUE, 0);
	/*
	GtkWidget *b_load, *b_make, *button_box;
	
	b_load = gtk_button_new_with_label("Load a map");
	b_make = gtk_button_new_with_label("Make a map");
	button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	
	gtk_container_add(GTK_CONTAINER(my->hbox1),button_box);
	gtk_container_add(GTK_CONTAINER(button_box),b_load);
	gtk_container_add(GTK_CONTAINER(button_box),b_make);
	
	g_signal_connect(b_load,"clicked",G_CALLBACK(on_item_loadmap_activate),my);
	g_signal_connect(b_make,"clicked",G_CALLBACK(on_item_editing_activate),my);
	*/
	gtk_widget_show_all(my->window);
	
}

void set_edit_mode(Mydata *my,int mode)
{
	if(mode < EDIT_NONE || mode > EDIT_LAST)
		return;
	my->edit_mode = mode;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(my->edit_radios[mode]),TRUE);
}

void set_bsp_mode(Mydata *my,int mode)
{
	if(mode < EDIT_NONE || mode > EDIT_LAST)
		return;
	my->bsp_mode = mode;
}

void on_edit_radio_toggled(GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);
	my->edit_mode = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget),"mode"));
}

void on_bsp_radio_toggled(GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);
	my->bsp_mode = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget),"mode"));
	refresh_area(my->area1);
}

void status_init(gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
	my->status = gtk_statusbar_new();
	set_status(my->status,"Welcome in GaBuZoMeu Racing !");
}

void editing_init(Mydata *my)
{
	GtkWidget *hbox;
	my->frame1 = gtk_frame_new("Editing");
	
	my->edit_radios[1] = gtk_radio_button_new_with_label_from_widget (NULL,"Add curve");
	my->edit_radios[2] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(my->edit_radios[1]),"Move curve");
	my->edit_radios[3] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(my->edit_radios[1]),"Remove curve");
	
	my->edit_radios[4] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(my->edit_radios[1]),"Add control");
	my->edit_radios[5] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(my->edit_radios[1]),"Move control");
	my->edit_radios[6] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(my->edit_radios[1]),"Remove control");
	
	hbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_container_add (GTK_CONTAINER(my->frame1), hbox);
	
	
	int i;
	for(i=EDIT_NONE+1;i<EDIT_LAST;i++)
	{
		gtk_box_pack_start (GTK_BOX (hbox), my->edit_radios[i], FALSE, FALSE, 0);
		g_signal_connect (my->edit_radios[i], "toggled",G_CALLBACK(on_edit_radio_toggled), my);
		g_object_set_data(G_OBJECT(my->edit_radios[i]),"mode",GINT_TO_POINTER(i));
	}
}
