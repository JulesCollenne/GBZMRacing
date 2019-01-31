#ifndef GAME_H
#define GAME_H
#define CAR_MAX 4
#define SAMPLE_MAX 400
#define LARGEUR_ROUTE 50
#define SPEED_MAX 6
#define CHAMPI_RADIUS 5

typedef enum { GS_HELLO, GS_EDIT, GS_PLAYING, GS_PAUSE, GS_WON, GS_LOST } Game_state;

typedef enum { GM_NONE, GM_SINGLE, GM_MULTI, GM_SERVER, GM_CLIENT } Game_mode;

typedef struct {
	double x,y,radius,angle,speed;
	int direction,accelere;
	double r,v,b;
	double compteur;
	int place;
} Car;

typedef struct {
	int sample_count;
	double sample_x[SAMPLE_MAX],sample_y[SAMPLE_MAX];
} Track;

typedef struct {
	Curve curve_left,curve_central,curve_right;
	Track track_left,track_central,track_right;
} Road;

typedef struct {
            Game_state state;
            Game_mode  mode;
            int score;
            Car cars[CAR_MAX];
            int car_count;
            Road road;     // par la suite un tableau par niveau
            int segrouge;
            double timer;
            int place;
            int nbChampis;
            double champis_x[SAMPLE_MAX/2],champis_y[SAMPLE_MAX/2];
            int champis_pris[SAMPLE_MAX/2];
        } Game;

void init_game(Game *game);
double get_horiz_angle_rad(double xa, double ya, double xb, double yb);
int compute_intersection(double xs0, double ys0, double xs1, double ys1, double xt0, double yt0, double xt1, double yt1, double *xp, double *yp);
void do_vector_product(double xa, double ya,double za,double xb, double yb,double zb,double *xn, double *yn,double *zn);
void compute_normal_right(double xa, double ya,double xb, double yb,double *xn, double *yn);
int point_is_on_right (double xa, double ya, double xb, double yb, double xc, double yc);
double compute_dist_point_to_line (double xa, double ya, double xb, double yb,double xc, double yc);
void draw_track (cairo_t *cr, Road *road);
void compute_road_tracks (Road *road, double theta);
void compute_bezier_track (Curve *curve, Track *tra, double theta);
void addCar(Game *game,double x,double y,double radius,double angle,double speed);
void progress_game_next_step (Game *game);
void place_cars_on_start (Game *game);
void copy_curve_infos_to_road (Curve_infos *ci, Road *road);
void compute_curves_road(Curve_infos *ci,double theta,Game *game);

#endif
