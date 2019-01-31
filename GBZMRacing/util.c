#include <gtk/gtk.h>
#include <stdlib.h>
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "util.h"
#include "area1.h"
#include "menus.h"
#include "gui.h"

//Met un status a la fenetre
void set_status (GtkWidget *status, const char *format, ...)
{
	char buf[1000];
	va_list ap;
	va_start (ap, format);
	vsnprintf (buf, sizeof(buf)-1, format, ap);
	va_end (ap);
	buf[sizeof(buf)-1] = 0;
	gtk_statusbar_pop  (GTK_STATUSBAR(status), 0);
	gtk_statusbar_push (GTK_STATUSBAR(status), 0, buf);
}

//Actualise la zone area
void refresh_area (GtkWidget *area)
{
	GdkWindow *win = gtk_widget_get_window (area);
	if (win == NULL) return;  // widget pas encore r ́ealis ́e
	gdk_window_invalidate_rect (win,NULL,FALSE); 
}

double min(double x,double y)
{
	if (x<y) return x;
	else return y;
}

double max(double x,double y)
{
	if (x>y) return x;
	else return y;
}
