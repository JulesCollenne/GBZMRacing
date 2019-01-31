#include <gtk/gtk.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "menus.h"
#include "gui.h"
#include "area1.h"
#include "font.h"

//Initialise le jeu
void init_game(Game *game)
{	
	srand(time(0));
	int i;
	game->nbChampis = 0;
	game->state = GS_HELLO;
    game->mode = GM_NONE;
    game->score = 0;
    game->timer = -3;
    for(i=0;i<CAR_MAX;i++)
    {
		game->cars[i].x = 50;
		game->cars[i].y = 50;
		game->cars[i].radius = 10;
		game->cars[i].angle = 0;
		game->cars[i].speed = 1;
		game->cars[i].compteur = -1;
		game->cars[i].place = -1;
		do{
			game->cars[i].r = (double)rand()/RAND_MAX*1.0;
			game->cars[i].v = (double)rand()/RAND_MAX*1.0;
			game->cars[i].b = (double)rand()/RAND_MAX*1.0;
		} while (game->cars[i].r == 0 && game->cars[i].v == 0 && game->cars[i].b == 0);
	}
    game->car_count = 0;
    game->segrouge = 0;
}

//Vecteur normal a droite
void compute_normal_right(double xa, double ya,double xb, double yb,double *xn, double *yn)
{
	double x = ya-yb;
	double y = xb-xa;
	double n = sqrt(x*x + y*y);
	if(n==0)
	{
		*xn = 0;
		*yn = 0;
	}
	else
	{
		*xn = x/n;
		*yn = y/n;
	}
}

//Produit vectoriel
void do_vector_product(double xa, double ya,double za,double xb, double yb,double zb,double *xn, double *yn,double *zn)
{
	*xn = ya*zb - yb*za;
	*yn = xa*zb - xb*za;
	*zn = xa*yb - xb*ya;
}

//Calcul l'intersection de s et t et le met dans p
int compute_intersection(double xs0, double ys0, double xs1, double ys1, double xt0, double yt0, double xt1, double yt1, double *xp, double *yp)
{
	double as,bs,cs,at,bt,ct,xi,yi,zi;
	do_vector_product(xs0,ys0,1,xs1,ys1,1,&as,&bs,&cs);
	do_vector_product(xt0,yt0,1,xt1,yt1,1,&at,&bt,&ct);
	do_vector_product(as,bs,cs,at,bt,ct,&xi,&yi,&zi);
	if(zi==0) return 0;
	else
	{
		*xp = xi/zi;
		*yp = yi/zi;
		return 1;
	}
}

//Calcul l'angle a,b en radians
double get_horiz_angle_rad(double xa, double ya, double xb, double yb)
{
	double dx, dy,n;
	dx = xb - xa;
	dy = yb - ya;
	n = sqrt(dx*dx+dy*dy);
	if(n==0)
		return 0;
	dx /= n;
	dy /= n;
	if(dy>0)
		return acos(dx);
	else
		return -acos(dx);
}

//Point est a droite ? True or false
int point_is_on_right (double xa, double ya, double xb, double yb, double xc, double yc)
{
	double res = (xb-xa)*(yc-ya)-(yb-ya)*(xc-xa);
	if (res > 0)
		return 1;
	else return 0;
}

//Calcule la distance a la ligne
double compute_dist_point_to_line (double xa, double ya, double xb, double yb,double xc, double yc)
{
	double dx,dy,dist,det;
	dx = xb-xa;
	dy = yb-ya;
	det = (xc-xa)*(yb-ya)-(xb-xa)*(yc-ya);
	dist = sqrt(dx*dx+dy*dy);
	return det/dist;
}

//Calcule les courbes de la route (droite,centre,gauche)
void compute_curves_road(Curve_infos *ci,double theta,Game *game)
{
	int i,j;
	double xn,yn,xt1,yt1,xt2,yt2;
	
	game->road.curve_central.control_count = ci->curve_list.curves[0].control_count;
	for(i=0;i<ci->curve_list.curves[0].control_count;i++)
		game->road.curve_central.controls[i] = ci->curve_list.curves[0].controls[i];
	
	game->road.curve_left.control_count = game->road.curve_central.control_count;
	game->road.curve_right.control_count = game->road.curve_central.control_count;
	
	compute_normal_right(game->road.curve_central.controls[0].x,game->road.curve_central.controls[0].y,game->road.curve_central.controls[1].x,game->road.curve_central.controls[1].y,&xn,&yn);
	game->road.curve_right.controls[0].x = game->road.curve_central.controls[0].x + xn * LARGEUR_ROUTE;
	game->road.curve_right.controls[0].y = game->road.curve_central.controls[0].y + yn * LARGEUR_ROUTE;
	game->road.curve_left.controls[0].x = game->road.curve_central.controls[0].x - xn * LARGEUR_ROUTE;
	game->road.curve_left.controls[0].y = game->road.curve_central.controls[0].y - yn * LARGEUR_ROUTE;
	
	for(j=1;j<game->road.curve_central.control_count-1;j++)
	{
		//game->road.curve_right
		compute_normal_right(game->road.curve_central.controls[j-1].x,game->road.curve_central.controls[j-1].y,game->road.curve_central.controls[j].x,game->road.curve_central.controls[j].y,&xn,&yn);
		xt1 = game->road.curve_central.controls[j].x + xn * LARGEUR_ROUTE;
		yt1 = game->road.curve_central.controls[j].y + yn * LARGEUR_ROUTE;
		
		compute_normal_right(game->road.curve_central.controls[j].x,game->road.curve_central.controls[j].y,game->road.curve_central.controls[j+1].x,game->road.curve_central.controls[j+1].y,&xn,&yn);
		xt2 = game->road.curve_central.controls[j].x + xn * LARGEUR_ROUTE;
		yt2 = game->road.curve_central.controls[j].y + yn * LARGEUR_ROUTE;
		
		game->road.curve_right.controls[j+1].x = game->road.curve_central.controls[j+1].x + xn;
		game->road.curve_right.controls[j+1].y = game->road.curve_central.controls[j+1].y + yn;
		
		compute_intersection(game->road.curve_right.controls[j-1].x,game->road.curve_right.controls[j-1].y,xt1,yt1,game->road.curve_right.controls[j+1].x,game->road.curve_right.controls[j+1].y,xt2,yt2,&game->road.curve_right.controls[j].x,&game->road.curve_right.controls[j].y);
		
		//game->road.curve_left
		compute_normal_right(game->road.curve_central.controls[j-1].x,game->road.curve_central.controls[j-1].y,game->road.curve_central.controls[j].x,game->road.curve_central.controls[j].y,&xn,&yn);
		xt1 = game->road.curve_central.controls[j].x - xn * LARGEUR_ROUTE;
		yt1 = game->road.curve_central.controls[j].y - yn * LARGEUR_ROUTE;
		
		compute_normal_right(game->road.curve_central.controls[j].x,game->road.curve_central.controls[j].y,game->road.curve_central.controls[j+1].x,game->road.curve_central.controls[j+1].y,&xn,&yn);
		xt2 = game->road.curve_central.controls[j].x - xn * LARGEUR_ROUTE;
		yt2 = game->road.curve_central.controls[j].y - yn * LARGEUR_ROUTE;
		
		game->road.curve_left.controls[j+1].x = game->road.curve_central.controls[j+1].x + xn;
		game->road.curve_left.controls[j+1].y = game->road.curve_central.controls[j+1].y + yn;
		
		compute_intersection(game->road.curve_left.controls[j-1].x,game->road.curve_left.controls[j-1].y,xt1,yt1,game->road.curve_left.controls[j+1].x,game->road.curve_left.controls[j+1].y,xt2,yt2,&game->road.curve_left.controls[j].x,&game->road.curve_left.controls[j].y);
	}
	compute_normal_right(game->road.curve_central.controls[j-1].x,game->road.curve_central.controls[j-1].y,game->road.curve_central.controls[j].x,game->road.curve_central.controls[j].y,&xn,&yn);
	game->road.curve_right.controls[j].x = game->road.curve_central.controls[j].x + xn * LARGEUR_ROUTE;
	game->road.curve_right.controls[j].y = game->road.curve_central.controls[j].y + yn * LARGEUR_ROUTE;
	
	game->road.curve_left.controls[j].x = game->road.curve_central.controls[j].x - xn * LARGEUR_ROUTE;
	game->road.curve_left.controls[j].y = game->road.curve_central.controls[j].y - yn * LARGEUR_ROUTE;
}

//Calcule la route en courbe de bézier
void compute_bezier_track (Curve *curve, Track *tra, double theta)
        {
            Control bez_points[4];

            tra->sample_count = 0;
            if (curve->control_count < 3) return;

            compute_bezier_points_prolong_first (curve, bez_points);
            sample_bezier_curve (bez_points, theta, tra->sample_x, tra->sample_y,&tra->sample_count, SAMPLE_MAX, 1);

            for (int k = 0; k < curve->control_count-3; k++) {
              compute_bezier_points_open (curve, k, bez_points);
              sample_bezier_curve (bez_points, theta, tra->sample_x, tra->sample_y, &tra->sample_count, SAMPLE_MAX, 0);
            }

            compute_bezier_points_prolong_last (curve, bez_points);
            sample_bezier_curve (bez_points, theta, tra->sample_x, tra->sample_y, &tra->sample_count, SAMPLE_MAX, 0);
        }

// Conversion des 3 curves en 3 tracks

        void compute_road_tracks (Road *road, double theta)
        {
            compute_bezier_track (&road->curve_left,    &road->track_left,    theta);
            compute_bezier_track (&road->curve_central, &road->track_central, theta);
            compute_bezier_track (&road->curve_right,   &road->track_right,   theta);
        }
		
		//Dessine la route, trait au centre
        void draw_track (cairo_t *cr, Road *road)
        {
			double dashes[1] = {50};
			cairo_set_source_rgb(cr,0.4,0.4,0.4);
			cairo_move_to (cr, road->track_left.sample_x[0], road->track_left.sample_y[0]);
			for (int k = 1; k < road->track_left.sample_count; k++)
				cairo_line_to (cr, road->track_left.sample_x[k], road->track_left.sample_y[k]);
			for (int k = road->track_right.sample_count-1; k >= 0; k--)
				cairo_line_to (cr, road->track_right.sample_x[k], road->track_right.sample_y[k]);
			
			cairo_fill (cr);
			cairo_set_line_width(cr,LARGEUR_ROUTE/10);
			cairo_set_dash(cr,dashes,1,1);
			
			cairo_set_source_rgb(cr,1,1,1);
			
            if (road->track_central.sample_count == 0) return;
            cairo_move_to (cr, road->track_central.sample_x[0], road->track_central.sample_y[0]);
            for (int k = 1; k < road->track_central.sample_count; k++)
              cairo_line_to (cr, road->track_central.sample_x[k], road->track_central.sample_y[k]);
            cairo_stroke (cr);            
        }
       
//Distance entre 1 et 2
double compute_distance_points(double x1,double y1,double x2,double y2)
{
	double resx = x2-x1;
	double resy = y2-y1;
	return sqrt(resx*resx+resy*resy);
}

//A chaque animation faire :
void progress_game_next_step (Game *game)
{
	int i,j;
	double boutx,bouty;
	double distcentre,distbout;
	double distarrive;
	for(i=0;i<game->car_count;i++)
	{
		
		while(i < game->car_count && game->cars[i].place != -1) i++;
		
		//Atteint la ligne d'arrivée
		distarrive = compute_dist_point_to_line(game->road.track_left.sample_x[game->road.track_left.sample_count-1],game->road.track_left.sample_y[game->road.track_left.sample_count-1],
		game->road.track_right.sample_x[game->road.track_right.sample_count-1],game->road.track_right.sample_y[game->road.track_right.sample_count-1],
		game->cars[i].x,game->cars[i].y);
		if(distarrive<0)
			distarrive *= -1.;
		if(game->cars[i].x > min(game->road.track_left.sample_x[game->road.track_left.sample_count-1],
		game->road.track_right.sample_x[game->road.track_right.sample_count-1])-game->cars[i].radius
		
		&& game->cars[i].y > min(game->road.track_left.sample_y[game->road.track_left.sample_count-1],
		game->road.track_right.sample_y[game->road.track_right.sample_count-1])-game->cars[i].radius 
		
		&& game->cars[i].x < max(game->road.track_left.sample_x[game->road.track_left.sample_count-1],
		game->road.track_right.sample_x[game->road.track_right.sample_count-1])+game->cars[i].radius 
		
		&& game->cars[i].y < max(game->road.track_left.sample_y[game->road.track_left.sample_count-1],
		game->road.track_right.sample_y[game->road.track_right.sample_count-1])+game->cars[i].radius
		&& distarrive < game->cars[i].radius
		&& game->cars[i].place == -1)
		{
			game->cars[i].compteur = game->timer;
			game->cars[i].place = game->place;
			game->cars[i].x = -20;
			game->cars[i].y = -20;
			game->place++;
		}
		
		switch(game->cars[i].direction){
			case 1 : game->cars[i].angle = game->cars[i].angle + 0.05;
			if (game->cars[i].angle >= 2*G_PI) game->cars[i].angle -= 2*G_PI;
			break;
			case -1 : game->cars[i].angle = game->cars[i].angle - 0.05;
			if (game->cars[i].angle < 0) game->cars[i].angle += 2*G_PI;
			break;
		}
		
		if (game->cars[i].accelere < 0 && game->cars[i].speed >= 0.2)
			game->cars[i].speed -= 0.2;
		else if(game->cars[i].accelere <= 0)
		{
			if(game->cars[i].speed <= 0.1 && game->cars[i].speed >= -0.1)
				game->cars[i].speed = 0;
				
			else if(game->cars[i].speed > 0.1)
				game->cars[i].speed -= 0.1;
			else if(game->cars[i].speed < 0.1)
				game->cars[i].speed += 0.1;
		}
		
		else if(game->cars[i].accelere > 0 && game->cars[i].speed <= SPEED_MAX-0.1)
		{
			game->cars[i].speed += 0.1;
		}
		else if(game->cars[i].accelere > 0 && game->cars[i].speed > SPEED_MAX)
			game->cars[i].speed -= 0.05;
		
		
		//Sortie de l'écran
		/*
		if(game->cars[i].x-game->cars[i].radius < 0 || game->cars[i].x+game->cars[i].radius > gtk_widget_get_allocated_width (my->area1) || game->cars[i].y-game->cars[i].radius < 0 || game->cars[i].y+game->cars[i].radius > gtk_widget_get_allocated_height (my->area1))
		{
			game->cars[i].speed = -0.25;;
			if(game->cars[i].angle < 0)
				game->cars[i].angle += 2*G_PI;
			else if(game->cars[i].angle >= 2*G_PI)
				game->cars[i].angle -= 2*G_PI;
		}
		*/
		//Collision bord gauche
		for(j=0;j<game->road.track_left.sample_count-1;j++)
		{
			if(game->cars[i].x > min(game->road.track_left.sample_x[j],game->road.track_left.sample_x[j+1])-game->cars[i].radius 
			&& game->cars[i].y > min(game->road.track_left.sample_y[j],game->road.track_left.sample_y[j+1])-game->cars[i].radius 
			&& game->cars[i].x < max(game->road.track_left.sample_x[j],game->road.track_left.sample_x[j+1])+game->cars[i].radius 
			&& game->cars[i].y < max(game->road.track_left.sample_y[j],game->road.track_left.sample_y[j+1])+game->cars[i].radius)
			 {
				 boutx = game->cars[i].x+cos(game->cars[i].angle);
				 bouty = game->cars[i].y+sin(game->cars[i].angle);
				 distcentre = compute_dist_point_to_line(game->road.track_left.sample_x[j],game->road.track_left.sample_y[j],game->road.track_left.sample_x[j+1],game->road.track_left.sample_y[j+1],game->cars[i].x,game->cars[i].y);
				 distbout = compute_dist_point_to_line(game->road.track_left.sample_x[j],game->road.track_left.sample_y[j],game->road.track_left.sample_x[j+1],game->road.track_left.sample_y[j+1],boutx,bouty);
				 
				 if(distcentre < distbout && game->cars[i].speed >= 0)
				{
					if(distcentre < 0)
					{
						if(distcentre >= -game->cars[i].radius)
						{
							if(game->cars[i].speed > 0)
							{
								game->cars[i].speed = 0.25;
								game->cars[i].angle = 2*get_horiz_angle_rad(game->road.track_left.sample_x[j],game->road.track_left.sample_y[j],game->road.track_left.sample_x[j+1],game->road.track_left.sample_y[j+1]) - game->cars[i].angle;
								if(game->cars[i].angle < 0)
									game->cars[i].angle += 2*G_PI;
								else if(game->cars[i].angle >= 2*G_PI)
									game->cars[i].angle -= 2*G_PI;
							}
						}
					}
				}
			 }
		}
		
		
		//Collision bord droit
		for(j=0;j<game->road.track_right.sample_count-1;j++)
		{
			if(game->cars[i].x > min(game->road.track_right.sample_x[j],game->road.track_right.sample_x[j+1])-game->cars[i].radius 
			&& game->cars[i].y > min(game->road.track_right.sample_y[j],game->road.track_right.sample_y[j+1])-game->cars[i].radius 
			&& game->cars[i].x < max(game->road.track_right.sample_x[j],game->road.track_right.sample_x[j+1])+game->cars[i].radius 
			&& game->cars[i].y < max(game->road.track_right.sample_y[j],game->road.track_right.sample_y[j+1])+game->cars[i].radius)
			 {
				 boutx = game->cars[i].x+cos(game->cars[i].angle);
				 bouty = game->cars[i].y+sin(game->cars[i].angle);
				 distcentre = compute_dist_point_to_line(game->road.track_right.sample_x[j],game->road.track_right.sample_y[j],game->road.track_right.sample_x[j+1],game->road.track_right.sample_y[j+1],game->cars[i].x,game->cars[i].y);
				 distbout = compute_dist_point_to_line(game->road.track_right.sample_x[j],game->road.track_right.sample_y[j],game->road.track_right.sample_x[j+1],game->road.track_right.sample_y[j+1],boutx,bouty);
				 
				 if(distcentre > distbout && game->cars[i].speed >= 0)
				{
					if(distcentre > 0)
					{
						if(distcentre <= game->cars[i].radius)
						{
							if(game->cars[i].speed > 0)
							{
								game->cars[i].speed = 0.25;
								game->cars[i].angle = 2*get_horiz_angle_rad(game->road.track_right.sample_x[j],game->road.track_right.sample_y[j],game->road.track_right.sample_x[j+1],game->road.track_right.sample_y[j+1]) - game->cars[i].angle;
								if(game->cars[i].angle < 0)
									game->cars[i].angle += 2*G_PI;
								else if(game->cars[i].angle >= 2*G_PI)
									game->cars[i].angle -= 2*G_PI;
							}
						}
					}
				}
			 }
		}
		
		//collision voiture
		double ang,diffang;
		for(j=i+1;j<game->car_count;j++)
		{
			diffang = game->cars[i].angle - game->cars[j].angle;
			if(diffang < 0)
				diffang *= -1;
			if(compute_distance_points(game->cars[i].x,game->cars[i].y,game->cars[j].x,game->cars[j].y) <= game->cars[i].radius + game->cars[j].radius)
			{
				if(diffang > G_PI/1.5)
				{
					ang = game->cars[i].angle;
					game->cars[i].angle = game->cars[j].angle;
					game->cars[j].angle = ang;
					game->cars[i].speed ++;
					game->cars[j].speed ++;
				}
				else
				{
					//Si c'est la voiture de derriere
					if(compute_distance_points(game->cars[i].x,game->cars[i].y,game->cars[j].x,game->cars[j].y) > 
					compute_distance_points(game->cars[i].x+cos(game->cars[i].angle)*game-> cars[i].radius,game->cars[i].y+sin(game->cars[i].angle)*game-> cars[i].radius,game->cars[j].x,game->cars[j].y))
					{
						game->cars[i].speed-= 1.25;
						game->cars[j].speed+= 1.25;
					}
					else
					{
						game->cars[j].speed-= 1.25;
						game->cars[i].speed+= 1.25;
					}
				}
			}
		}
		
		//Champi
		for(j=0;j<game->nbChampis;j++)
		{
			if(compute_distance_points(game->cars[i].x,game->cars[i].y,game->champis_x[j],game->champis_y[j]) < game->cars[i].radius+ CHAMPI_RADIUS && !game->champis_pris[j])
			{
				game->cars[i].speed+=3;
				game->champis_pris[j] = 1;
			}
		}
		
		game->cars[i].x += game->cars[i].speed*cos(game->cars[i].angle);
		game->cars[i].y += game->cars[i].speed*sin(game->cars[i].angle);
		
	}
	
	if(game->place == game->car_count+1)
	{
		game->state = GS_WON;
		game->timer = -5.;
	}
	
}

void place_cars_on_start (Game *game)
{
	game->timer = -3;
	int i;
	double vecx,vecy,dist;
	game->place = 1;
	for(i=0;i<game->car_count;i++)
	{
		game->cars[i].speed = 0;
		game->cars[i].direction = 0;
		game->cars[i].accelere = 0;
		game->cars[i].place = -1;
		game->cars[i].compteur = -1.;
		vecx = game->road.curve_left.controls[0].x - game->road.curve_right.controls[0].x;
		vecy = game->road.curve_left.controls[0].y - game->road.curve_right.controls[0].y;
		dist = sqrt(vecx*vecx+vecy*vecy);
		vecx /= dist;
		vecy /= dist;
		game->cars[i].x = game->road.curve_central.controls[0].x+vecx*(i-1.5)*(game->cars[i].radius*2+5);
		game->cars[i].y = game->road.curve_central.controls[0].y+vecy*(i-1.5)*(game->cars[i].radius*2+5);
		game->cars[i].angle = get_horiz_angle_rad(game->road.curve_central.controls[0].x,game->road.curve_central.controls[0].y,game->road.curve_central.controls[1].x,game->road.curve_central.controls[1].y); //.........A changer
	}
	int alea;
	game->nbChampis = 0;
	for(i=0;i<game->road.track_central.sample_count && game->nbChampis<SAMPLE_MAX/2;i++)
	{
		alea = rand()%game->road.track_central.sample_count;
		if(alea < 3)
		{
			game->nbChampis++;
			game->champis_x[game->nbChampis-1] = game->road.track_central.sample_x[i] + (double)(rand()/RAND_MAX*LARGEUR_ROUTE/2-LARGEUR_ROUTE/4);
			game->champis_y[game->nbChampis-1] = game->road.track_central.sample_y[i] + (double)(rand()/RAND_MAX*LARGEUR_ROUTE/2-LARGEUR_ROUTE/4);
			game->champis_pris[game->nbChampis-1] = 0;
		}
	}
}

void copy_curve_infos_to_road (Curve_infos *ci, Road *road)
{
	
}
