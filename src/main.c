#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>
#include <cairo.h>
#include <math.h>

#include "bubble.h"

static GList *bubbles = NULL;
static gint main_id;
static gint num;

static gboolean
main_loop (gpointer data)
{
  gint i;
  Bubble *tmp = NULL;
  for (i = 0; i < num; i++) {
    tmp = g_list_nth_data (bubbles, i);
    if (tmp->y <= tmp->radius 
        || tmp->y >= SCREEN_HEIGHT - tmp->radius) {

      tmp->vspeed = -tmp->vspeed;
      tmp->y = (tmp->y > SCREEN_HEIGHT / 2 
                  ? SCREEN_HEIGHT - (tmp->radius * 2) : tmp->radius * 2);
    }

    if (tmp->x <= tmp->radius 
        || tmp->x >= SCREEN_WIDTH - tmp->radius) {

      tmp->hspeed = -tmp->hspeed;
      tmp->x = (tmp->x > SCREEN_WIDTH / 2 
                  ? SCREEN_WIDTH - (tmp->radius * 2) : tmp->radius * 2);
    }

    tmp->x += tmp->hspeed;
    tmp->y += tmp->vspeed;
    clutter_actor_set_position (CLUTTER_ACTOR (tmp->actor), tmp->x, tmp->y);
  }

}
static void
load_bubbles (gint number) 
{
  gint i;
  for (i = 0; i < number; i++) {
    bubbles = g_list_append (bubbles, bubblechain_bubble_new (i));
  } 
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
  //clutter_stage_hide_cursor (CLUTTER_STAGE (stage));

  num = 5;
  load_bubbles (num);
    
  gtk_container_add (GTK_CONTAINER (window), clutter_widget);

  gint i;
  Bubble *tmp = NULL;
  for (i = 0; i < num; i++) {
    tmp = g_list_nth_data (bubbles, i);
    clutter_container_add_actor (CLUTTER_CONTAINER (stage), 
                                 tmp->actor);
  }
  
  main_id = g_timeout_add (20, (GSourceFunc) main_loop, NULL);

  g_signal_connect (window, "hide", G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show_all (window);

  gtk_main ();
  return 1;
}
