#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>
#include <cairo.h>
#include <math.h>

#include "bubble.h"

static Bubble* bubble;

static gboolean
main_loop (gpointer data)
{
  
}

int
main (gint argc, gchar **argv)
{
  gtk_clutter_init (&argc, &argv);

  GtkWidget *window;
  GtkWidget *clutter_widget;
  ClutterActor *stage;
  ClutterColor stage_color = {0x00, 0x00, 0x00, 0xff};

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "BubbleChain");
  gtk_window_set_default_size (GTK_WINDOW (window), 680, 480);

  clutter_widget = gtk_clutter_embed_new ();
  stage = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter_widget));
  clutter_stage_set_color (CLUTTER_STAGE (stage), &stage_color);
  clutter_stage_hide_cursor (CLUTTER_STAGE (stage));

  bubble = bubblechain_bubble_new ();
    
  gtk_container_add (GTK_CONTAINER (window), clutter_widget);

  clutter_container_add_actor (CLUTTER_CONTAINER (stage), bubble->actor);
 
  g_signal_connect (window, "hide", G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show_all (window);

  gtk_main ();
  return 1;
}
