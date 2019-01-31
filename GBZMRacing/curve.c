#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "util.h"
#include "area1.h"
#include "menus.h"
#include "gui.h"

void convert_bsp3_to_bezier(double p[4], double b[4])
{
	b[0] = (1/6.)*(p[0]+4*p[1]+p[2]);
	b[1] = (1/6.)*(4*p[1]+2*p[2]);
	b[2] = (1/6.)*(2*p[1]+4*p[2]);
	b[3] = (1/6.)*(p[1]+4*p[2]+p[3]);
}

void convert_bsp3_to_bezier_prolong_first(double p[3],double b[4])
{
	b[0] = p[0];
	b[1] = (2*p[0]+p[1])/3.;
	b[2] = (p[0]+2*p[1])/3.;
	b[3] = (p[0] + 4*p[1] + p[2])/6.;
}

void convert_bsp3_to_bezier_prolong_last(double p[3],double b[4])
{
	b[0] = (p[0] + 4*p[1] + p[2])/6.;
	b[1] = (2*p[1] + p[2])/3.;
	b[2] = (p[1] + 2*p[2])/3.;
	b[3] = p[2];
}

void compute_bezier_points_open(Curve *curve, int i, Control bez_points[4])
{
	int j;
	double px[4],py[4],bx[4],by[4];
	for(j=i;j<i+4;j++)
	{
			px[j-i] = curve->controls[j].x;
			py[j-i] = curve->controls[j].y;
			
	}
	convert_bsp3_to_bezier(px,bx);
	convert_bsp3_to_bezier(py,by);
	for(j=0;j<4;j++)
	{
		bez_points[j].x = bx[j];
		bez_points[j].y = by[j];
	}
}

void compute_bezier_points_close(Curve *curve, int i, Control bez_points[4])
{
	int j;
	double px[4],py[4],bx[4],by[4];
	
	if(i>=curve->control_count-3)
	{
		for(j=i;j<i+4;j++)
		{
			px[j-i] = curve->controls[j%curve->control_count].x;
			py[j-i] = curve->controls[j%curve->control_count].y;
		}
	}
	else
	{
		for(j=i;j<i+4;j++)
		{
			px[j-i] = curve->controls[j].x;
			py[j-i] = curve->controls[j].y;
		}
	}
	convert_bsp3_to_bezier(px,bx);
	convert_bsp3_to_bezier(py,by);
	for(j=0;j<4;j++)
	{
		bez_points[j].x = bx[j];
		bez_points[j].y = by[j];
	}
}

void compute_bezier_points_prolong_first(Curve *curve, Control bez_points[4])
{	
	int j;
	double px[4],py[4],bx[4],by[4];
	for(j=0;j<4;j++)
	{
			px[j] = curve->controls[j].x;
			py[j] = curve->controls[j].y;
			
	}
	convert_bsp3_to_bezier_prolong_first(px,bx);
	convert_bsp3_to_bezier_prolong_first(py,by);
	for(j=0;j<4;j++)
	{
		bez_points[j].x = bx[j];
		bez_points[j].y = by[j];
	}
}

void compute_bezier_points_prolong_last(Curve *curve, Control bez_points[4])
{	
	int j;
	double px[4],py[4],bx[4],by[4];
	for(j=0;j<4;j++)
	{
			px[j] = curve->controls[j+curve->control_count-3].x;
			py[j] = curve->controls[j+curve->control_count-3].y;
			
	}
	convert_bsp3_to_bezier_prolong_last(px,bx);
	convert_bsp3_to_bezier_prolong_last(py,by);
	for(j=0;j<4;j++)
	{
		bez_points[j].x = bx[j];
		bez_points[j].y = by[j];
	}
}

double compute_bezier_cubic (double b[4], double t)
{
	return (1.-t)*(1.-t)*(1.-t)*b[0] + 3*(1.-t)*(1.-t)*t*b[1] + 3*(1-t)*t*t*b[2] + t*t*t*b[3];
}

void init_curve_infos(Curve_infos *ci)
{
	ci->curve_list.curve_count = 0;
	ci->current_curve = -1;
	ci->current_control = -1;
}

int add_curve(Curve_infos *ci)
{
	if(ci->curve_list.curve_count >= CURVE_MAX)
	{
		ci->current_curve = -1;
		return -1;
	}
	
	int n = ci->curve_list.curve_count;
	ci->curve_list.curve_count++;
	ci->curve_list.curves[n].control_count = 0;
	ci->current_curve = n;
	ci->current_control = -1;
	ci->curve_list.curves[n].shift_x = 0;
	ci->curve_list.curves[n].shift_y = 0;
	return n;
}

int add_control(Curve_infos *ci,double x, double y)
{
	int n = ci->current_curve;
	if(n<0 || n>ci->curve_list.curve_count-1)
		return -1;
	
	if(ci->curve_list.curves[n].control_count>=CONTROL_MAX)
	{
		ci->current_control = -1;
		return -1;
	}
	
	int k = ci->curve_list.curves[n].control_count;
	ci->curve_list.curves[n].control_count++;
	
	ci->curve_list.curves[n].controls[k].x = x;
	ci->curve_list.curves[n].controls[k].y = y;
	ci->current_control = k;
	return k;
}

int find_control(Curve_infos *ci, double x, double y)
{
	int i,j;
	double dx,dy;
	for(i=0;i<ci->curve_list.curve_count;i++)
	{
		for(j=0;j<ci->curve_list.curves[i].control_count;j++)
		{
			if(x>ci->curve_list.curves[i].controls[j].x)
				dx = x-ci->curve_list.curves[i].controls[j].x;
			else
				dx = ci->curve_list.curves[i].controls[j].x-x;
			if(y>ci->curve_list.curves[i].controls[j].y)
				dy = y-ci->curve_list.curves[i].controls[j].y;
			else
				dy = ci->curve_list.curves[i].controls[j].y-y;
			if(dx*dx + dy*dy <= 25)
			{
				ci->current_curve = i;
				ci->current_control = j;
				return 0;
			}
		}
	}
	return -1;
}

int move_control(Curve_infos *ci,double dx,double dy)
{
	if(!(ci->current_curve >= 0 && ci->current_curve < ci->curve_list.curve_count && ci->current_control >= 0 && ci->current_control < ci->curve_list.curves[ci->current_curve].control_count))
		return -1;
	ci->curve_list.curves[ci->current_curve].controls[ci->current_control].x += dx;
	ci->curve_list.curves[ci->current_curve].controls[ci->current_control].y += dy;
	return 0;
}

int move_curve(Curve_infos *ci,double dx,double dy)
{
	int i;
	if(!(ci->current_curve >= 0 && ci->current_curve < ci->curve_list.curve_count))
		return -1;
	for(i=0;i<ci->curve_list.curves[ci->current_curve].control_count;i++)
	{
		ci->curve_list.curves[ci->current_curve].controls[i].x += dx;
		ci->curve_list.curves[ci->current_curve].controls[i].y += dy;
	}
	return 0;
}

int remove_curve(Curve_infos *ci)
{
	if(ci->current_curve == -1)
		return -1;
	int n = ci->current_curve;
	memmove(ci->curve_list.curves+n,ci->curve_list.curves+n+1,sizeof(Curve)*(ci->curve_list.curve_count-1-n));
	ci->curve_list.curve_count--;
	ci->current_curve = -1;
	return 0;
}

int remove_control(Curve_infos *ci)
{
	if(ci->current_curve == -1 || ci->curve_list.curves[ci->current_curve].control_count == -1 || ci->curve_list.curves[ci->current_curve].control_count <= ci->current_control)
		return -1;
	int n = ci->current_curve;
	int m = ci->current_control;
	if(ci->curve_list.curves[n].control_count == 1)
	{
		memmove(ci->curve_list.curves[n].controls+m-1,ci->curve_list.curves[n].controls+m,sizeof(Control)*(ci->curve_list.curves[n].control_count-1-m));
		remove_curve(ci);
	}
	else
	{
		memmove(ci->curve_list.curves[n].controls+m,ci->curve_list.curves[n].controls+m+1,sizeof(Control)*(ci->curve_list.curves[n].control_count-1-m));
		ci->curve_list.curves[n].control_count--;
	}
	ci->current_control = -1;
	return 0;
}

/* Échantillonnage et stockage des courbes */

        void store_sample (double x, double y,double sx[], double sy[], int *ind, int ind_max)
        {
          if (*ind >= ind_max) {
            fprintf (stderr, "%s: capacity exceeded \n", __func__);
            return;
          }
          sx[*ind] = x;
          sy[*ind] = y;
          *ind += 1;
        }

/* Échantillonne une courbe de Bézier avec le pas théta.
         * Stocke les points dans sx[0..ind_max-1], sy[0..ind_max-1] à partir de ind. 
         * Au retour, ind est le nouveau point d'insertion.
         * Fct inspirée de area1.c:generate_bezier_path() du TP6 (supprimée ici)
        */
        void sample_bezier_curve (Control bez_points[4], double theta,double sx[], double sy[], int *ind, int ind_max, int is_first)
        {
          double x, y, bx[4], by[4], t;

          for (int j = 0; j <= 3 ; j++) {
              bx[j] = bez_points[j].x;
              by[j] = bez_points[j].y;
          }

          for (t = is_first ? 0.0 : theta; t < 1.0; t += theta) {
            x = compute_bezier_cubic (bx, t);
            y = compute_bezier_cubic (by, t);
            store_sample (x, y, sx, sy, ind, ind_max);
          }

          if (t < 1.0) {
            x = compute_bezier_cubic (bx, 1.0);
            y = compute_bezier_cubic (by, 1.0);
            store_sample (x, y, sx, sy, ind, ind_max);
          }
        }
