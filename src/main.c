#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>
#include <cairo.h>
#include <math.h>

#include "bubble.h"

static GList *bubbles = NULL;
static GList *bursted_bubbles = NULL;
static ClutterActor *group = NULL;
static gint main_id;
static ClutterActor *stage = NULL;

static gboolean
burst_bubble (gpointer data)
{
  Bubble *tmp = (Bubble *)data;

  clutter_actor_animate (tmp->actor, CLUTTER_EASE_IN_BOUNCE, 700,
                         "scale-x", 0.0, "scale-y", 0.0,
                         "fixed::scale-gravity", CLUTTER_GRAVITY_CENTER,
                         NULL);
  bursted_bubbles = g_list_remove (bursted_bubbles, tmp);
}

static gboolean
main_loop (gpointer data)
{
  gint i, j, count, c_count;
  Bubble *tmp = NULL;
  Bubble *bursted_tmp = NULL;
  Bubble *new_bursted_tmp = NULL;
  gfloat dist;

  count = g_list_length (bubbles);

  for (i = 0; i < count; i++) {
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
    bubblechain_bubble_move (tmp, tmp->x, tmp->y);

    c_count = g_list_length (bursted_bubbles);
    for (j = 0; j < c_count; j++) {
      bursted_tmp = g_list_nth_data (bursted_bubbles, j);
      
      dist = fabs (sqrt (pow ((bursted_tmp->x_c - tmp->x_c), 2)
                        + pow ((bursted_tmp->y_c - tmp->y_c), 2)));
      if (dist <= (bursted_tmp->radius + tmp->radius)) {
        tmp->bursted = TRUE;
        new_bursted_tmp = bubblechain_bubble_new (-j, BUBBLE_BURSTED);
        bubblechain_bubble_move (new_bursted_tmp, 
                                tmp->x_c - new_bursted_tmp->radius, 
                                tmp->y_c - new_bursted_tmp->radius);
        clutter_actor_set_scale (new_bursted_tmp->actor, 0.3, 0.3);
        clutter_container_add_actor (CLUTTER_CONTAINER (stage), 
                                     new_bursted_tmp->actor);
        clutter_actor_animate (new_bursted_tmp->actor, 
                               CLUTTER_EASE_OUT_BOUNCE, 900,
                               "scale-x", 1.0, "scale-y", 1.0,
                               "fixed::scale-gravity", CLUTTER_GRAVITY_CENTER,
                               NULL);
        bursted_bubbles = g_list_prepend (bursted_bubbles, new_bursted_tmp);
        clutter_actor_hide (tmp->actor);
        bubbles = g_list_remove (bubbles, tmp);
        count--;
        g_timeout_add (2500, (GSourceFunc) burst_bubble, new_bursted_tmp);
        break;
      }
    }
  }

  return TRUE;
}

static void
load_bubbles (gint number) 
{
  gint i;
  Bubble *tmp = NULL;

  for (i = 0; i < number; i++) {
    tmp = bubblechain_bubble_new (i, BUBBLE_NORMAL);
    clutter_container_add_actor (CLUTTER_CONTAINER (group), tmp->actor);
    bubbles = g_list_prepend (bubbles, tmp);
  }
}

static gboolean
on_button_press (ClutterActor *actor, ClutterEvent *event, gpointer data)
{
  ClutterButtonEvent *ev = (ClutterButtonEvent*)event;
  Bubble *new = bubblechain_bubble_new (-1, BUBBLE_BURSTED);

  clutter_container_add_actor (CLUTTER_CONTAINER (actor), new->actor);
  bubblechain_bubble_move (new, ev->x, ev->y);

  bursted_bubbles = g_list_prepend (bursted_bubbles, new);
  g_timeout_add (2500, (GSourceFunc) burst_bubble, new);
  return FALSE;
}

static gboolean
on_motion_event (ClutterActor *actor, ClutterEvent *event, gpointer data)
{
  Bubble *tmp = (Bubble*)data;
  ClutterMotionEvent *mev = (ClutterMotionEvent *) event;
  clutter_actor_set_position (tmp->actor, mev->x, mev->y);
  tmp->x = mev->x;
  tmp->y = mev->y;

  return FALSE;
}

int
main (gint argc, gchar **argv)
{
  gtk_clutter_init (&argc, &argv);

  GtkWidget *window;
  GtkWidget *clutter_widget;
  ClutterActor *background;
  ClutterColor stage_color = {0x00, 0x00, 0x00, 0xff};
  Bubble *big_bubble;

  group = clutter_group_new ();

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "BubbleChain");
  gtk_window_set_default_size (GTK_WINDOW (window), 680, 480);

  clutter_widget = gtk_clutter_embed_new ();
  gtk_container_add (GTK_CONTAINER (window), clutter_widget);
  stage = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter_widget));
  clutter_stage_set_color (CLUTTER_STAGE (stage), &stage_color);
  clutter_stage_hide_cursor (CLUTTER_STAGE (stage));

  background = clutter_rectangle_new_with_color (&stage_color);
  clutter_actor_set_position (background, 0, 0);
  clutter_actor_set_size (background, 680,480);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), background);
  clutter_actor_lower_bottom (background);

  gint num = 65;
  load_bubbles (num);

  clutter_container_add_actor (CLUTTER_CONTAINER (stage), 
                               CLUTTER_ACTOR (group));
  
  big_bubble = bubblechain_bubble_new (1, BUBBLE_BURSTED);
  clutter_actor_set_position (big_bubble->actor, 
                              SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), big_bubble->actor);

  main_id = g_timeout_add (20, (GSourceFunc) main_loop, NULL);

  g_signal_connect (window, "hide", G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect (stage, "button-press-event", 
                    G_CALLBACK (on_button_press), big_bubble);
  g_signal_connect (stage, "motion-event", 
                    G_CALLBACK (on_motion_event), big_bubble);

  gtk_widget_show_all (window);

  gtk_main ();
  return 1;
}
