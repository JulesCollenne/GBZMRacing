#ifndef AREA1_H
#define AREA1_H

void apply_image_transforms (Mydata *my);
void update_area1_with_transforms(Mydata *my);
void draw_curves(cairo_t *cr,Curve_infos *ci);
void area1_init(gpointer user_data);
void draw_bezier_polygons_open(cairo_t *cr, Curve_infos *ci);
void draw_control_polygons(cairo_t *cr,Curve_infos *ci);
void draw_bezier_curve(cairo_t *cr, Control bez_points[4], double theta);
void draw_bezier_curves_open(cairo_t *cr,Curve_infos *ci,double theta);
void draw_bezier_curves_close(cairo_t *cr,Curve_infos *ci,double theta);
void draw_bezier_curves_prolong(cairo_t *cr,Curve_infos *ci,double theta);
void draw_bezier_curves_fill (cairo_t *cr, Curve_infos *ci, double theta);
void draw_bezier_curves_clip (cairo_t *cr, Curve_infos *ci, double theta, Mydata *my);
void draw_road(cairo_t *cr,Curve_infos *ci,double theta,Game *game);
void draw_bezier_curves_track(cairo_t *cr,double theta,Game *game,Mydata *my);
void set_anim1_mode (Mydata *my, int flag);
gboolean on_timeout1 (gpointer data);
void draw_playing_gui(cairo_t *cr,double theta,Game *game,Mydata *my);
void draw_menu(cairo_t *cr,Mydata *my);
#endif
