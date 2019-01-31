#include <gtk/gtk.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "util.h"
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "menus.h"
#include "gui.h"
#include "area1.h"
#include "font.h"

#define ANIM1_TIME 20

//Applique les modifications du pixbuf1
void apply_image_transforms (Mydata *my)
{
	g_clear_object(&my->pixbuf2);
	if(my->pixbuf1==NULL)return;
	GdkPixbuf *tmp1 = gdk_pixbuf_rotate_simple(my->pixbuf1,my->rotate_angle);
	my->pixbuf2 = gdk_pixbuf_scale_simple(tmp1,gdk_pixbuf_get_width(tmp1)*my->scale1_value,gdk_pixbuf_get_height(tmp1)*my->scale1_value,GDK_INTERP_BILINEAR);
	g_object_unref (tmp1);
}

//Actualise la zone de dessin
void update_area1_with_transforms(Mydata *my)
{
	apply_image_transforms(my);
	if(my->pixbuf1==NULL)return;
	
	gtk_widget_set_size_request (my->area1, gdk_pixbuf_get_width(my->pixbuf2), gdk_pixbuf_get_height(my->pixbuf2));
	refresh_area(my->area1);
	
}

//Dessine les points de controle et les segments
void draw_control_polygons(cairo_t *cr,Curve_infos *ci)
{
	int i,j,xtemp = ci->curve_list.curves[0].controls[0].x, ytemp = ci->curve_list.curves[0].controls[0].y,nouv = 0;
	for(i=0;i<ci->curve_list.curve_count;i++)
	{
		for(j=0;j<ci->curve_list.curves[i].control_count;j++)
		{
			//Points
			if(ci->current_curve == i && ci->current_control == j)
				cairo_set_source_rgb(cr,1.0,0.0,0.0);
			else
				cairo_set_source_rgb(cr,0.0,0.0,1.0);
			cairo_rectangle (cr, ci->curve_list.curves[i].controls[j].x-3, ci->curve_list.curves[i].controls[j].y-3, 6.0, 6.0);
			cairo_stroke (cr);
			
			//Segments
			if(nouv == 0)
			{
				if(ci->current_curve == i)
					cairo_set_source_rgb(cr,1.0,1.0,0.0);
				else
					cairo_set_source_rgb(cr,0.75,0.75,0.75);
				cairo_move_to (cr,xtemp, ytemp);
				cairo_line_to(cr,ci->curve_list.curves[i].controls[j].x, ci->curve_list.curves[i].controls[j].y);
				cairo_stroke (cr);
			}
			xtemp = ci->curve_list.curves[i].controls[j].x;
			ytemp = ci->curve_list.curves[i].controls[j].y;
			nouv = 0;
		}
		nouv = 1;
	}
}

//Dessine le menu du début
void draw_menu(cairo_t *cr,Mydata *my)
{
	int w = gtk_widget_get_allocated_width (my->area1),h = gtk_widget_get_allocated_height (my->area1);
	int lt,ht;
	cairo_set_source_rgb(cr,1,0.1,0.1);
	PangoLayout *layout = pango_cairo_create_layout (cr);
	font_set_name(layout,"Lato, Black,50");
	pango_layout_set_text (layout,"GaBuZoMeu Racing",-1);
	pango_layout_get_pixel_size (layout, &lt, &ht);
	cairo_move_to (cr, w/2 - lt/2,  0);
	pango_cairo_show_layout (cr, layout);
	
	g_object_unref(layout);
}

//Signal du dessin (actualisation fenetre)
gboolean on_area1_draw(GtkApplication* app, cairo_t *cr, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
	if(my->pixbuf2!=NULL) 
	{
		double pixbuf_width = gdk_pixbuf_get_width(my->pixbuf2);
		double pixbuf_height = gdk_pixbuf_get_height(my->pixbuf2);
		cairo_rectangle(cr,0,0,pixbuf_width,pixbuf_height);
		gdk_cairo_set_source_pixbuf(cr,my->pixbuf2,0,0);
		cairo_fill(cr);
	}
	
	if(my->game.state == GS_HELLO)
	{
		draw_menu(cr,my);
	}
	if(my->game.state == GS_EDIT)
	{
		if(my->bsp_mode != BSP_TRACK)
		{
			draw_control_polygons(cr,&my->curve_infos);
			draw_bezier_polygons_open(cr,&my->curve_infos);
		}
		switch(my->bsp_mode){
			case BSP_ROAD :
				draw_road(cr,&my->curve_infos,0.1,&my->game);
			break;
		}
	}
	else if (my->game.state == GS_PLAYING || my->game.state == GS_WON)
	{
		//draw_track(cr,&my->game.road);
		draw_bezier_curves_track(cr,0.1,&my->game,my);
		refresh_area(my->area1);
	}
	else if (my->game.state == GS_HELLO)
	{
		draw_track(cr,&my->game.road);
		refresh_area(my->area1);
	}
	if(my->game.state == GS_WON)
		set_anim1_mode(my,FALSE);
	return TRUE;
}

//Touche du clavier enfoncée
gboolean on_area1_key_press(GtkWidget *widget,GdkEvent *event, gpointer data)
{
	Mydata *my = get_mydata(data);
	GdkEventKey *evk = &event->key;
	//printf ("%s: GDK_KEY_%s\n",__func__, gdk_keyval_name(evk->keyval));
	switch (evk->keyval) {
		case GDK_KEY_Escape : on_item_quit_activate(widget,my); break;
		case GDK_KEY_p : on_item_rotate_activate(widget,my); break;
		case GDK_KEY_ampersand : set_edit_mode(my,1); break;
		case GDK_KEY_eacute : set_edit_mode(my,2); break;
		case GDK_KEY_quotedbl : set_edit_mode(my,3); break;
		case GDK_KEY_apostrophe : set_edit_mode(my,4); break;
		case GDK_KEY_parenleft : set_edit_mode(my,5); break;
		case GDK_KEY_minus : set_edit_mode(my,6); break;
		
		case GDK_KEY_Left  : my->flag_key_left[0]  = 1; break;
        case GDK_KEY_Right : my->flag_key_right[0] = 1; break;
        case GDK_KEY_Up : my->flag_key_up[0] = 1; break;
        case GDK_KEY_Down : my->flag_key_down[0] = 1; break;
        
        case GDK_KEY_q  : my->flag_key_left[1]  = 1; break;
        case GDK_KEY_d : my->flag_key_right[1] = 1; break;
        case GDK_KEY_z : my->flag_key_up[1] = 1; break;
        case GDK_KEY_s : my->flag_key_down[1] = 1; break;
        
        case GDK_KEY_k  : my->flag_key_left[2]  = 1; break;
        case GDK_KEY_m : my->flag_key_right[2] = 1; break;
        case GDK_KEY_o : my->flag_key_up[2] = 1; break;
        case GDK_KEY_l : my->flag_key_down[2] = 1; break;
        
        case GDK_KEY_KP_4  : my->flag_key_left[3]  = 1; break;
        case GDK_KEY_KP_6 : my->flag_key_right[3] = 1; break;
        case GDK_KEY_KP_8 : my->flag_key_up[3] = 1; break;
        case GDK_KEY_KP_5 : my->flag_key_down[3] = 1; break;
	}
	return TRUE;
}

//Touche du clavier relachée
gboolean on_area1_key_release(GtkWidget *widget,GdkEvent *event, gpointer data)
{	Mydata *my = get_mydata(data);
	GdkEventKey *evk = &event->key;
	switch (evk->keyval) {
		case GDK_KEY_Left  : my->flag_key_left[0]  = 0; break;
		case GDK_KEY_Right : my->flag_key_right[0] = 0; break;
		case GDK_KEY_Up : my->flag_key_up[0] = 0; break;
		case GDK_KEY_Down : my->flag_key_down[0] = 0; break;
		
		case GDK_KEY_q  : my->flag_key_left[1]  = 0; break;
        case GDK_KEY_d : my->flag_key_right[1] = 0; break;
        case GDK_KEY_z : my->flag_key_up[1] = 0; break;
        case GDK_KEY_s : my->flag_key_down[1] = 0; break;
        
        case GDK_KEY_k  : my->flag_key_left[2]  = 0; break;
        case GDK_KEY_m : my->flag_key_right[2] = 0; break;
        case GDK_KEY_o : my->flag_key_up[2] = 0; break;
        case GDK_KEY_l : my->flag_key_down[2] = 0; break;
        
        case GDK_KEY_KP_4  : my->flag_key_left[3]  = 0; break;
        case GDK_KEY_KP_6 : my->flag_key_right[3] = 0; break;
        case GDK_KEY_KP_8 : my->flag_key_up[3] = 0; break;
        case GDK_KEY_KP_5 : my->flag_key_down[3] = 0; break;
	}
	return TRUE;
}

//Clique souris enfoncé
gboolean on_area1_button_press(GtkWidget *widget,GdkEvent *event, gpointer data)
{
	Mydata *my = get_mydata(data);
	GdkEventButton *evb = &event->button;
	my->click_n = evb->button;
	my->click_x = evb->x;
	my->click_y = evb->y;
	if(evb->button == 1 && my->show_edit)
	{
		switch (my->edit_mode) {
			case EDIT_ADD_CURVE : 
				if(add_curve(&my->curve_infos) < 0) return TRUE;
				set_edit_mode(my,EDIT_ADD_CONTROL);
				add_control(&my->curve_infos,evb->x,evb->y);
			break;
			
			case EDIT_MOVE_CURVE :
				find_control(&my->curve_infos,evb->x,evb->y);
			break;
			case EDIT_REMOVE_CURVE :
				if (find_control(&my->curve_infos,evb->x,evb->y) == 0)
					remove_curve(&my->curve_infos);
			break;
			case EDIT_ADD_CONTROL :
				add_control(&my->curve_infos,evb->x,evb->y);
			break;
			
			case EDIT_MOVE_CONTROL :
				find_control(&my->curve_infos,evb->x,evb->y);
			break;
			
			case EDIT_REMOVE_CONTROL :
			if (find_control(&my->curve_infos,evb->x,evb->y) == 0)
				remove_control(&my->curve_infos);
			break;
		}
	}
	refresh_area(my->area1);
	return TRUE;
}

//Clique souris relaché
gboolean on_area1_button_release(GtkWidget *widget,GdkEvent *event, gpointer data)
{
	Mydata *my = get_mydata(data);
	my->click_n = 0;
	refresh_area(my->area1);
	return TRUE;
}

//Deplacement souris
gboolean on_area1_motion_notify(GtkWidget *widget,GdkEvent *event, gpointer data)
{
	Mydata *my = get_mydata(data);
	GdkEventMotion *evm = &event->motion;
	my->last_x = my->click_x ; my->last_y = my->click_y;
	my->click_x = evm->x ; my->click_y = evm->y;
	if(my->click_n == 1 && my->show_edit)
	{
		switch (my->edit_mode) {
			case EDIT_ADD_CURVE : 
			break;
			
			case EDIT_MOVE_CURVE :
				move_curve(&my->curve_infos,my->click_x-my->last_x,my->click_y-my->last_y);
			break;
			case EDIT_REMOVE_CURVE :  break;
			case EDIT_ADD_CONTROL :
			break;
			
			case EDIT_MOVE_CONTROL :
				find_control(&my->curve_infos,evm->x,evm->y);
				move_control(&my->curve_infos,my->click_x-my->last_x,my->click_y-my->last_y);
			break;
			
			case EDIT_REMOVE_CONTROL :  break;

		}
	}
	refresh_area(my->area1);
	return TRUE;
}

//Souris entre dans l'area1
gboolean on_area1_enter_notify(GtkWidget *widget,GdkEvent *event, gpointer data)
{	
	Mydata *my = get_mydata(data);
	gtk_widget_grab_focus (my->area1);
	return TRUE;
}

//Souris sors d'area1
gboolean on_area1_leave_notify(GtkWidget *widget,GdkEvent *event, gpointer data)
{
	
	return TRUE;
}

//Dessine les polygones ouvert
void draw_bezier_polygons_open(cairo_t *cr, Curve_infos *ci)
{
	int i,j;
	Control bez_points[4];
	for(i=0;i<ci->curve_list.curve_count;i++)
	{
		for(j=0;j<ci->curve_list.curves[i].control_count-3;j++)
		{
			compute_bezier_points_open(&ci->curve_list.curves[i],j,bez_points);
			cairo_set_source_rgb(cr,0.0,1.0,0.0);
			cairo_move_to (cr,bez_points[0].x, bez_points[0].y);
			cairo_line_to (cr,bez_points[1].x, bez_points[1].y);
			cairo_stroke (cr);
			cairo_move_to (cr,bez_points[2].x, bez_points[2].y);
			cairo_line_to (cr,bez_points[3].x, bez_points[3].y);
			cairo_stroke (cr);
		}
	}
}

//Dessine les courbes de bézier
void draw_bezier_curve(cairo_t *cr, Control bez_points[4], double theta)
{
	int i;
	double bx[4],by[4],t,xtemp1 = 0,ytemp1 = 0,xtemp2,ytemp2;
	for(i=0;i<4;i++)
	{
		bx[i] = bez_points[i].x;
		by[i] = bez_points[i].y;
	}
	cairo_set_source_rgb(cr,1.0,0.0,0.0);
	for(t=0;t<1;t+=theta)
	{
		xtemp2 = compute_bezier_cubic(bx,t);
		ytemp2 = compute_bezier_cubic(by,t);
		if (xtemp1 != 0 && ytemp1 != 0)
		{
			cairo_move_to (cr,xtemp1, ytemp1);
			cairo_line_to (cr,xtemp2,ytemp2);
			cairo_stroke (cr);
		}
		xtemp1 = xtemp2;
		ytemp1 = ytemp2;
	}
}

//Dessine les courbe de bézier ouverte
void draw_bezier_curves_open(cairo_t *cr,Curve_infos *ci,double theta)
{
	int i,k;
	Control bez_points[4];
	for(k=0;k<ci->curve_list.curve_count;k++)
	{
		for(i=0;i<ci->curve_list.curves[k].control_count-3;i++)
		{
			compute_bezier_points_open(&ci->curve_list.curves[k],i,bez_points);
			draw_bezier_curve(cr,bez_points,theta);
		}
	}
}

//Dessine les courbe de bézier fermée
void draw_bezier_curves_close(cairo_t *cr,Curve_infos *ci,double theta)
{
	int i,k;
	Control bez_points[4];
	for(k=0;k<ci->curve_list.curve_count;k++)
	{
		for(i=0;i<ci->curve_list.curves[k].control_count;i++)
		{
			compute_bezier_points_close(&ci->curve_list.curves[k],i,bez_points);
			draw_bezier_curve(cr,bez_points,theta);
		}
	}
}

//Dessine les courbe de bézier prolongée
void draw_bezier_curves_prolong(cairo_t *cr,Curve_infos *ci,double theta)
{
	int i,k;
	Control bez_points[4];
	for(k=0;k<ci->curve_list.curve_count;k++)
	{
		if(ci->curve_list.curves[k].control_count>3)
		{
			compute_bezier_points_prolong_first(&ci->curve_list.curves[k],bez_points);
			draw_bezier_curve(cr,bez_points,theta);
			for(i=0;i<ci->curve_list.curves[k].control_count-3;i++)
			{
				compute_bezier_points_open(&ci->curve_list.curves[k],i,bez_points);
				draw_bezier_curve(cr,bez_points,theta);
			}
			compute_bezier_points_prolong_last(&ci->curve_list.curves[k],bez_points);
			draw_bezier_curve(cr,bez_points,theta);
		}
	}
}

//Dessine la route
void draw_road(cairo_t *cr,Curve_infos *ci,double theta,Game *game)
{
	int i;
	Control bez_points[4],bpoints_r[4],bpoints_l[4];
	
	compute_curves_road(ci,theta,game);
	
	if(game->road.curve_central.control_count>3)
	{	
		
		compute_bezier_points_prolong_first(&game->road.curve_central,bez_points);
		draw_bezier_curve(cr,bez_points,theta);
	
		compute_bezier_points_prolong_first(&game->road.curve_right,bpoints_r);
		draw_bezier_curve(cr,bpoints_r,theta);
		
		compute_bezier_points_prolong_first(&game->road.curve_left,bpoints_l);
		draw_bezier_curve(cr,bpoints_l,theta);
		
		for(i=0;i<game->road.curve_central.control_count-3;i++)
		{
			compute_bezier_points_open(&game->road.curve_central,i,bez_points);
			draw_bezier_curve(cr,bez_points,theta);
			
			compute_bezier_points_open(&game->road.curve_right,i,bpoints_r);
			draw_bezier_curve(cr,bpoints_r,theta);
			
			compute_bezier_points_open(&game->road.curve_left,i,bpoints_l);
			draw_bezier_curve(cr,bpoints_l,theta);
				
		}
		compute_bezier_points_prolong_last(&game->road.curve_central,bez_points);
		draw_bezier_curve(cr,bez_points,theta);
		
		compute_bezier_points_prolong_last(&game->road.curve_left,bpoints_l);
		draw_bezier_curve(cr,bpoints_l,theta);
		
		compute_bezier_points_prolong_last(&game->road.curve_right,bpoints_r);
		draw_bezier_curve(cr,bpoints_r,theta);
	}
}

//Calcul et dessine la route et le menu de course
void draw_bezier_curves_track(cairo_t *cr,double theta,Game *game,Mydata *my)
{
	compute_road_tracks(&game->road,theta);
	
	draw_track(cr,&game->road);
	
	draw_playing_gui(cr,theta,game,my);
	
}

//Dessine les voitures, le timer et les champis
void draw_playing_gui(cairo_t *cr,double theta,Game *game,Mydata *my)
{		
	int i;
	//Voitures
	PangoLayout *layout = pango_cairo_create_layout (cr);
	for(i=0;i<game->car_count;i++)
	{
		cairo_set_source_rgb(cr,game->cars[i].r,game->cars[i].v,game->cars[i].b);
		cairo_arc(cr,game->cars[i].x,game->cars[i].y,game->cars[i].radius,0,G_PI*2);
		cairo_fill(cr);
	
		cairo_set_source_rgb(cr,0,0,0);
		cairo_move_to(cr,game->cars[i].x,game->cars[i].y);
		cairo_line_to(cr,game->cars[i].x+cos(game->cars[i].angle)*10,game->cars[i].y+sin(game->cars[i].angle)*10);
		cairo_stroke(cr);
		cairo_set_source_rgb(cr,game->cars[i].r,game->cars[i].v,game->cars[i].b);
		font_set_name(layout,"Lato, Black,10");
		font_draw_text (cr, layout, FONT_MC, game->cars[i].x, game->cars[i].y-20,"%d", i+1);
	}
	
	for(i=0;i<game->nbChampis;i++)
	{
		if(!game->champis_pris[i])
		{
			cairo_set_source_rgb(cr,1,0,0);
			cairo_arc(cr,game->champis_x[i],game->champis_y[i],CHAMPI_RADIUS,0,G_PI*2);
			cairo_fill(cr);
		}
	}
	/* DEBUG Segments rouges pour les collisions
	cairo_set_source_rgb(cr,1,0,0);
	cairo_move_to(cr,game->road.track_left.sample_x[game->segrouge],game->road.track_left.sample_y[game->segrouge]);
	cairo_line_to(cr,game->road.track_left.sample_x[game->segrouge+1],game->road.track_left.sample_y[game->segrouge+1]);
	cairo_stroke(cr);
	*/
	
	int w = gtk_widget_get_allocated_width (my->area1),h = gtk_widget_get_allocated_height (my->area1);
	font_set_name(layout,"Lato, Black,100");
	int lt,ht;
	cairo_set_source_rgb(cr,1,0,0);
	if(game->state == GS_PLAYING)
	{
		if(game->timer < -2)
		{
			pango_layout_set_text (layout,"3",-1);
			pango_layout_get_pixel_size (layout, &lt, &ht);
			cairo_move_to (cr, w/2 - lt/2., h/2 - ht/2.);
			pango_cairo_show_layout (cr, layout);
		}
		
		else if(game->timer < -1)
		{
			pango_layout_set_text (layout,"2",-1);
			pango_layout_get_pixel_size (layout, &lt, &ht);
			cairo_move_to (cr, w/2 - lt/2., h/2 - ht/2.);
			pango_cairo_show_layout (cr, layout);
		}
		
		else if(game->timer < 0)
		{
			pango_layout_set_text (layout,"1",-1);
			pango_layout_get_pixel_size (layout, &lt, &ht);
			cairo_move_to (cr, w/2 - lt/2., h/2 - ht/2.);
			pango_cairo_show_layout (cr, layout);
		}
			
		else if(game->timer < 1)
		{
			pango_layout_set_text (layout,"GO",-1);
			pango_layout_get_pixel_size (layout, &lt, &ht);
			cairo_move_to (cr, w/2 - lt/2., h/2 - ht/2.);
			pango_cairo_show_layout (cr, layout);
		}
	}
	if(game->timer > 0)
	{
		font_set_name(layout,"Lato, Black,25");
		font_draw_text (cr, layout, FONT_TL, w/2, 0,"%.2lf", game->timer);
	}
	
	for(i=0;i<game->car_count;i++)
	{
		if(game->cars[i].compteur > 0)
		{
			font_set_name(layout,"Lato, Black,25");
			if (game->cars[i].place == 1)
				font_draw_text (cr, layout, FONT_TR, w-10, 0+(game->cars[i].place-1)*30,"%dst | Car %d : %.2lfsec", game->cars[i].place,i+1,game->cars[i].compteur);
			else if(game->cars[i].place == 2)
				font_draw_text (cr, layout, FONT_TR, w-10, 0+(game->cars[i].place-1)*30,"%dnd | Car %d : %.2lfsec", game->cars[i].place,i+1,game->cars[i].compteur);
			else if(game->cars[i].place == 3)
				font_draw_text (cr, layout, FONT_TR, w-10, 0+(game->cars[i].place-1)*30,"%drd | Car %d : %.2lfsec", game->cars[i].place,i+1,game->cars[i].compteur);
			else if(game->cars[i].place == 4)
				font_draw_text (cr, layout, FONT_TR, w-10, 0+(game->cars[i].place-1)*30,"%dth | Car %d : %.2lfsec", game->cars[i].place,i+1,game->cars[i].compteur);
		}
	}
	g_object_unref(layout);
}

//Genere le passage du bézier
void generate_bezier_path (cairo_t *cr, Control bez_points[4], double theta, int is_first){
	int i;
	double bx[4],by[4],t,x = 0,y = 0;
	for(i=0;i<4;i++)
	{
		bx[i] = bez_points[i].x;
		by[i] = bez_points[i].y;
	}
	
	x = compute_bezier_cubic(bx,0);
	y = compute_bezier_cubic(by,0);
	
	if (is_first)
		cairo_move_to (cr,x,y);
	
	for(t=theta;t<1;t+=theta)
	{
		x = compute_bezier_cubic(bx,t);
		y = compute_bezier_cubic(by,t);
		if (x != 0 && y != 0)
			cairo_line_to (cr,x,y);
	}
}

//Remplis la selection de la courbe de bézier en rouge
void draw_bezier_curves_fill (cairo_t *cr, Curve_infos *ci, double theta){
	int i,k;
	Control bez_points[4];
	for(k=0;k<ci->curve_list.curve_count;k++)
	{
		compute_bezier_points_close(&ci->curve_list.curves[k],0,bez_points);
		generate_bezier_path(cr,bez_points,theta,TRUE);
		for(i=1;i<ci->curve_list.curves[k].control_count;i++)
		{
			compute_bezier_points_close(&ci->curve_list.curves[k],i,bez_points);
			generate_bezier_path(cr,bez_points,theta,FALSE);
		}
		cairo_set_source_rgb(cr,1.0,0.0,0.0);
		cairo_fill(cr);
	}
}

//Dessine l'image de fond mais seulement la partie dans la courbe de bézier
void draw_bezier_curves_clip (cairo_t *cr, Curve_infos *ci, double theta, Mydata *my){
	int i,k;
	Control bez_points[4];
	if(my->pixbuf2 != NULL) gdk_cairo_set_source_pixbuf(cr,my->pixbuf2,ci->curve_list.curves[ci->current_curve].shift_x,ci->curve_list.curves[ci->current_curve].shift_y);
	for(k=0;k<ci->curve_list.curve_count;k++)
	{
		compute_bezier_points_close(&ci->curve_list.curves[k],0,bez_points);
		generate_bezier_path(cr,bez_points,theta,TRUE);
		for(i=1;i<ci->curve_list.curves[k].control_count;i++)
		{
			compute_bezier_points_close(&ci->curve_list.curves[k],i,bez_points);
			generate_bezier_path(cr,bez_points,theta,FALSE);
		}
		cairo_fill(cr);
	}
}

//Active ou desactive l'animation
void set_anim1_mode (Mydata *my, int flag)
        {
            if (flag) {
                if (my->timeout1 != 0) return;
                my->timeout1 = g_timeout_add (ANIM1_TIME, on_timeout1, my);

                set_status (my->status, "Game started");
            } else {
                if (my->timeout1 == 0) return;
                g_source_remove (my->timeout1);
                my->timeout1 = 0;

                set_status (my->status, "Game stopped");
            }
        }
        
//A chaque animation, faire :
gboolean on_timeout1 (gpointer data)
        {
          Mydata *my = get_mydata(data);
          Game *game = &my->game;
          int i;
		my->game.timer+=0.02;
		if(my->game.state == GS_PLAYING)
		{
			if(my->game.timer >= 0)
				  {
				  for(i=0;i<my->game.car_count;i++)
				  {
					  game->cars[i].direction =(my->flag_key_left[i]  && !my->flag_key_right[i]) ? -1 :(my->flag_key_right[i] && !my->flag_key_left[i] ) ?  1 : 0;
					  game->cars[i].accelere = (my->flag_key_up[i]  && !my->flag_key_down[i]) ? 1 :(my->flag_key_down[i] && !my->flag_key_up[i] ) ?  -1 : 0;
				  }
				  progress_game_next_step (&my->game);
				}
		}
		return TRUE;
	}

//Initialise l'area1
void area1_init(gpointer user_data)
{
	Mydata *my = get_mydata(user_data);

	my->area1 = gtk_drawing_area_new();
	
	g_signal_connect (my->area1, "draw",G_CALLBACK(on_area1_draw), my);
	g_signal_connect (my->area1, "key-press-event",G_CALLBACK(on_area1_key_press), my);
	g_signal_connect (my->area1, "key-release-event",G_CALLBACK(on_area1_key_release), my);
	g_signal_connect (my->area1, "button-press-event",G_CALLBACK(on_area1_button_press), my);
	g_signal_connect (my->area1, "button-release-event",G_CALLBACK(on_area1_button_release), my);
	g_signal_connect (my->area1, "motion-notify-event",G_CALLBACK(on_area1_motion_notify), my);
	g_signal_connect (my->area1, "enter-notify-event",G_CALLBACK(on_area1_enter_notify), my);
	g_signal_connect (my->area1, "leave-notify-event",G_CALLBACK(on_area1_leave_notify), my);
}

