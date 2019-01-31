#include <gtk/gtk.h>
#include <stdlib.h>
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "util.h"
#include "area1.h"
#include "menus.h"
#include "gui.h"

void on_item_about_activate(GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);
	char *auteur[] = {"Adrien Durouge <adrien.durouge@etu.univ-amu.fr>", "Jules Collenne <jules.collenne@etu.univ-amu.fr>", NULL};
	gtk_show_about_dialog(NULL,"program-name",my->title,"version","2.4","website","http://j.mp/optigra","authors",auteur,"logo-icon-name","face-monkey",NULL);
}

void on_item_rotate_activate(GtkWidget *widget, gpointer data)
{
	Mydata *my = data;
	if(my->pixbuf1 == NULL) return;
	my->rotate_angle+=90;
	if(my->rotate_angle>=360)my->rotate_angle-=360;
	update_area1_with_transforms(my);
	set_status(my->status,"Image rotated");
}

void on_item_editing_activate(GtkWidget *widget, gpointer data)
{
	Mydata *my = data;
	my->game.state = GS_EDIT;
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
	{
		my->show_edit = TRUE;
		set_status(my->status,"Editing is on");
		gtk_widget_show(my->frame1);
	}
	else
	{
		my->show_edit = FALSE;
		set_status(my->status,"Editing is off");
		gtk_widget_hide(my->frame1);
	}
}

void on_item_bg_color_activate (GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);
	GtkWidget *bg_dialog = GTK_WIDGET(gtk_color_chooser_dialog_new("Background color",NULL));
	GdkRGBA bg_color;
	char *name_color = malloc(sizeof(char)*100);
	if(gtk_dialog_run(GTK_DIALOG(bg_dialog))==(int)GTK_RESPONSE_OK){
		gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(bg_dialog),&bg_color);
		name_color = (char *)gdk_rgba_to_string(&bg_color);
		set_status(my->status,"Selected bg color : %s",name_color);
	}
	gtk_widget_destroy(bg_dialog);
	gtk_widget_override_background_color(my->area1,GTK_STATE_FLAG_NORMAL,&bg_color);
}

void on_item_load_activate (GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);
	my->scale1_value = 1.0;
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Open File",GTK_WINDOW(my->window),GTK_FILE_CHOOSER_ACTION_OPEN,"Cancel", GTK_RESPONSE_CANCEL,"Open", GTK_RESPONSE_ACCEPT,NULL);
	
    my->scale1 = NULL;


	if(my->current_folder!=NULL) gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),my->current_folder);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		set_status(my->status,"Loading image...");
		g_clear_object(&my->pixbuf1);
		my->pixbuf1 = gdk_pixbuf_new_from_file(filename,NULL);

		if(my->pixbuf1 == NULL)
		{
			set_status(my->status,"Loading failed : not an image");
		}

		else
		{
			set_status(my->status,"Loading success %dx%d",gdk_pixbuf_get_width(my->pixbuf1),gdk_pixbuf_get_height(my->pixbuf1));
			my->scale1_value = 1;
			my->rotate_angle = 0;
			update_area1_with_transforms(my);
		}
		g_free (filename);
		my->current_folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER (dialog));
	}

	gtk_widget_destroy (dialog);
}

void on_item_scale_activate (GtkWidget *widget,gpointer data)
{
	Mydata *my = get_mydata(data);
	gtk_window_present(GTK_WINDOW(my->win_scale));
	gtk_widget_show_all (my->win_scale);
	set_status(my->status,"Scale : %lf",my->scale1_value);
}

void on_item_quit_activate (GtkWidget *widget,gpointer data)
{
	Mydata *my = get_mydata(data);
	gtk_widget_destroy(my->window);
}

void on_item_1P_activate (GtkWidget *widget,gpointer data)
{
	Mydata *my = get_mydata(data);
	Game *game = &my->game;
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(my->item_editing),FALSE);
	game->state = GS_PLAYING;
	game->car_count = 1;
	my->show_edit = FALSE;
	gtk_widget_hide(my->frame1);

    if (game->state == GS_PLAYING) {
            copy_curve_infos_to_road (&my->curve_infos, &game->road);
            compute_road_tracks (&game->road,0.1);
            place_cars_on_start (game);
            set_anim1_mode (my, TRUE);
        } else {
            set_anim1_mode (my, FALSE);
        }
        refresh_area (my->area1);
}

void on_item_2P_activate (GtkWidget *widget,gpointer data)
{
	Mydata *my = get_mydata(data);
	Game *game = &my->game;
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(my->item_editing),FALSE);
	game->state = GS_PLAYING;
	game->car_count = 2;
	my->show_edit = FALSE;
	gtk_widget_hide(my->frame1);

    if (game->state == GS_PLAYING) {
            copy_curve_infos_to_road (&my->curve_infos, &game->road);
            compute_road_tracks (&game->road,0.1);
            place_cars_on_start (game);
            set_anim1_mode (my, TRUE);
        } else {
            set_anim1_mode (my, FALSE);
        }
        refresh_area (my->area1);
}

void on_item_3P_activate (GtkWidget *widget,gpointer data)
{
	Mydata *my = get_mydata(data);
	Game *game = &my->game;
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(my->item_editing),FALSE);
	game->state = GS_PLAYING;
	game->car_count = 3;
	my->show_edit = FALSE;
	gtk_widget_hide(my->frame1);

    if (game->state == GS_PLAYING) {
            copy_curve_infos_to_road (&my->curve_infos, &game->road);
            compute_road_tracks (&game->road,0.1);
            place_cars_on_start (game);
            set_anim1_mode (my, TRUE);
        } else {
            set_anim1_mode (my, FALSE);
        }
        refresh_area (my->area1);
}

void on_item_4P_activate (GtkWidget *widget,gpointer data)
{
	Mydata *my = get_mydata(data);
	Game *game = &my->game;
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(my->item_editing),FALSE);
	game->state = GS_PLAYING;
	game->car_count = 4;
	my->show_edit = FALSE;
	gtk_widget_hide(my->frame1);

    if (game->state == GS_PLAYING) {
            copy_curve_infos_to_road (&my->curve_infos, &game->road);
            compute_road_tracks (&game->road,0.1);
            place_cars_on_start (game);
            set_anim1_mode (my, TRUE);
        } else {
            set_anim1_mode (my, FALSE);
        }
        refresh_area (my->area1);
}

void on_item_loadmap_activate (GtkWidget *widget,gpointer data)
{
	Mydata *my = get_mydata(data);
	char *filename;
	int i;
	my->game.state = GS_EDIT;
	add_curve(&my->curve_infos);
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Open File",GTK_WINDOW(my->window),GTK_FILE_CHOOSER_ACTION_OPEN,"Cancel", GTK_RESPONSE_CANCEL,"Open", GTK_RESPONSE_ACCEPT,NULL);
	if(my->current_folder!=NULL) gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),my->current_folder);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		FILE *fichier = fopen(filename,"r+");
		if (fichier == NULL)
			return;
		if(fscanf(fichier,"%d",&my->curve_infos.curve_list.curves[0].control_count) == 0);
		for(i=0;i<my->curve_infos.curve_list.curves[0].control_count;i++)
		{
			if(fscanf(fichier,"%lf %lf",&my->curve_infos.curve_list.curves[0].controls[i].x,&my->curve_infos.curve_list.curves[0].controls[i].y) == 0);
		}
		fclose(fichier);
	}
	gtk_widget_destroy (dialog);
	compute_curves_road(&my->curve_infos,0.1,&my->game);
	compute_road_tracks(&my->game.road,0.1);
}

void on_item_savemap_activate (GtkWidget *widget,gpointer data)
{
	Mydata *my = get_mydata(data);
	Game *game = &my->game;
	int i;
	char *filename;
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Open File",GTK_WINDOW(my->window),GTK_FILE_CHOOSER_CONFIRMATION_ACCEPT_FILENAME,"Cancel", GTK_RESPONSE_CANCEL,"Open", GTK_RESPONSE_ACCEPT,NULL);
	if(my->current_folder!=NULL) gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),my->current_folder);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		FILE* fichier = fopen(filename,"w+");
		fprintf(fichier,"%d\n",game->road.curve_central.control_count);
		for(i=0;i<game->road.curve_central.control_count;i++)
		{
			fprintf(fichier,"%lf %lf\n",game->road.curve_central.controls[i].x,game->road.curve_central.controls[i].y);
		}
		fclose(fichier);
	}
	gtk_widget_destroy (dialog);
}
void menus_init(gpointer user_data)
{
	Mydata *my = get_mydata(user_data);

	GtkWidget *item_file, *item_tools, *item_help, *sub_file, *sub_tools, *sub_help, *item_quit, *item_rotate, *item_bg_color, *item_about, *item_scale;
	GtkWidget *item_1P, *item_2P, *item_3P,*item_4P,*item_loadmap,*item_savemap, *item_load;
	my->menu_bar = gtk_menu_bar_new();

	item_file = gtk_menu_item_new_with_label("File");
	gtk_menu_shell_append (GTK_MENU_SHELL(my->menu_bar), item_file);

	item_tools = gtk_menu_item_new_with_label("Tools");
	gtk_menu_shell_append (GTK_MENU_SHELL(my->menu_bar), item_tools);

	item_help = gtk_menu_item_new_with_label("Help");
	gtk_menu_shell_append (GTK_MENU_SHELL(my->menu_bar), item_help);

	sub_file = gtk_menu_new();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_file), sub_file);

	sub_tools = gtk_menu_new();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_tools), sub_tools);

	sub_help = gtk_menu_new();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_help), sub_help);

	item_1P = gtk_menu_item_new_with_label("1 Player");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_file), item_1P);
	
	item_2P = gtk_menu_item_new_with_label("2 Players");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_file), item_2P);
	
	item_3P = gtk_menu_item_new_with_label("3 Players");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_file), item_3P);
	
	item_4P = gtk_menu_item_new_with_label("4 Players");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_file), item_4P);

	item_loadmap = gtk_menu_item_new_with_label("Load map");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_file), item_loadmap);
	
	item_savemap = gtk_menu_item_new_with_label("Save map");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_file), item_savemap);

	item_quit = gtk_menu_item_new_with_label("Quit");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_file), item_quit);

	item_rotate = gtk_menu_item_new_with_label("Rotate bg");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_tools), item_rotate);

	item_scale = gtk_menu_item_new_with_label("Scale bg");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_tools), item_scale);

	item_bg_color = gtk_menu_item_new_with_label("Bg color");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_tools), item_bg_color);

	item_load = gtk_menu_item_new_with_label("Load bg image");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_tools), item_load);

	item_about = gtk_menu_item_new_with_label("About");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_help), item_about);
	
	my->item_editing = gtk_check_menu_item_new_with_label("Editing");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_tools), my->item_editing);
	
	g_signal_connect (item_1P, "activate",G_CALLBACK(on_item_1P_activate), my);
	g_signal_connect (item_2P, "activate",G_CALLBACK(on_item_2P_activate), my);
	g_signal_connect (item_3P, "activate",G_CALLBACK(on_item_3P_activate), my);
	g_signal_connect (item_4P, "activate",G_CALLBACK(on_item_4P_activate), my);
	g_signal_connect (item_loadmap, "activate",G_CALLBACK(on_item_loadmap_activate), my);
	g_signal_connect (item_savemap, "activate",G_CALLBACK(on_item_savemap_activate), my);
	g_signal_connect (item_quit, "activate",G_CALLBACK(on_item_quit_activate), my);
	g_signal_connect (item_rotate, "activate",G_CALLBACK(on_item_rotate_activate), my);
	g_signal_connect (item_about, "activate",G_CALLBACK(on_item_about_activate), my);
	g_signal_connect (item_bg_color, "activate",G_CALLBACK(on_item_bg_color_activate), my);
	g_signal_connect (item_load, "activate",G_CALLBACK(on_item_load_activate), my);
	g_signal_connect (item_scale, "activate",G_CALLBACK(on_item_scale_activate), my);
	g_signal_connect (my->item_editing, "activate",G_CALLBACK(on_item_editing_activate), my);
}
