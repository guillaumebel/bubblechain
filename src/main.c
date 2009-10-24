#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>
#include <cairo.h>
#include <math.h>

#include "bubble.h"

static gint num_bubbles = 5;
static gint current_level = 0;
static gint bubble_hit;
static gint score;
static gint level[] = {1, 2, 4, 6, 10, 15, 18, 22, 30, 37, 48, 55};
static gboolean player_clicked = FALSE;

static Bubble *big_bubble = NULL;
static GList *bubbles = NULL;
static GList *bursted_bubbles = NULL;
static ClutterActor *bubbles_group = NULL;
static gint main_id;

static ClutterActor *stage = NULL;
static ClutterColor color_success = {0x00,0x19,0x35,0x99};
static ClutterColor color_normal = {0x10,0x11,0x10,0xff};

static ClutterActor *score_label;
static ClutterActor *bubbles_count;
static ClutterActor *level_label;
static ClutterActor *message_label;

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

static void
load_bubbles (gint number) 
{
  gint i;
  Bubble *tmp = NULL;

  while (bubbles) {
    tmp = g_list_last (bubbles)->data;
    clutter_actor_hide (tmp->actor);
    bubbles = g_list_delete_link (bubbles, g_list_last (bubbles));
  }

  bubble_hit = 0;

  for (i = 0; i < number; i++) {
    tmp = bubblechain_bubble_new (i, BUBBLE_NORMAL);
    clutter_container_add_actor (CLUTTER_CONTAINER (bubbles_group), tmp->actor);
    bubbles = g_list_prepend (bubbles, tmp);
  }

  gchar *text = g_strdup_printf ("%d bubbles out of %d", level[current_level], number);
  clutter_actor_set_position (level_label, SCREEN_WIDTH / 2 -
                              clutter_actor_get_width (level_label) / 2, 3);
  clutter_text_set_text (CLUTTER_TEXT (level_label), text);
  text = g_strdup_printf ("%d", 0);
  clutter_text_set_text (CLUTTER_TEXT (bubbles_count), text);
  g_free (text);

}

static void
update_scoreboard (gint new_score)
{
  score++;
  score += new_score * 100;
  gchar *text = g_strdup_printf ("Score: %d", score);
  clutter_text_set_text (CLUTTER_TEXT (score_label), text);
  text = g_strdup_printf ("%d", new_score);
  clutter_text_set_text (CLUTTER_TEXT (bubbles_count), text);
  g_free (text);
}

static void
show_message (const gchar* message)
{
  clutter_text_set_text (CLUTTER_TEXT (message_label), message);
  clutter_actor_set_position (message_label, (SCREEN_WIDTH / 2) -
                              (clutter_actor_get_width (message_label) / 2),
                              SCREEN_HEIGHT -
                              clutter_actor_get_height (message_label) * 1.5);
  clutter_actor_set_opacity (message_label, 0x00);
  clutter_actor_animate (message_label, CLUTTER_LINEAR, 900,
                         "opacity", 0xCC,
                         NULL);
}

static void 
set_next_level (void) {
  if (bubble_hit >= level[current_level]) {
      current_level++;

      if (current_level > 12) {
        current_level = 0;
        num_bubbles = 0;
      }
      num_bubbles += 5;
      load_bubbles (num_bubbles);
      clutter_actor_animate (stage, CLUTTER_LINEAR, 600,
                           "color", &color_normal,
                           NULL);

      bubble_hit = 0;
    } else {
      load_bubbles (num_bubbles);
    }
    player_clicked = FALSE;
    clutter_actor_show (CLUTTER_ACTOR (big_bubble->actor));
    clutter_stage_hide_cursor (CLUTTER_STAGE (stage));
}

static void
animate_bursted_bubbles (Bubble *b, gfloat x_c, gfloat y_c) 
{
  bubblechain_bubble_move (b, x_c - b->radius, y_c - b->radius);
  clutter_actor_set_scale (b->actor, 0.3, 0.3);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), b->actor);
  clutter_actor_animate (b->actor, 
                         CLUTTER_EASE_OUT_BOUNCE, 900,
                         "scale-x", 1.0, "scale-y", 1.0,
                         "fixed::scale-gravity", CLUTTER_GRAVITY_CENTER,
                         NULL);
}

static gboolean
main_loop (gpointer data)
{
  gint i, j;
  Bubble *tmp = NULL;
  Bubble *bursted_tmp = NULL;
  Bubble *new_bursted_tmp = NULL;
  gfloat dist;

  //Collision detection for every bubbles
  for (i = 0; i < g_list_length (bubbles); i++) {
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

    //Move the bubbles
    tmp->x += tmp->hspeed;
    tmp->y += tmp->vspeed;
    bubblechain_bubble_move (tmp, tmp->x, tmp->y);
    
    //Collision detectoin between bubbles and bursted bubbles
    for (j = 0; j < g_list_length (bursted_bubbles); j++) {
      bursted_tmp = g_list_nth_data (bursted_bubbles, j);
      
      dist = fabs (sqrt (pow ((bursted_tmp->x_c - tmp->x_c), 2)
                        + pow ((bursted_tmp->y_c - tmp->y_c), 2)));

      if (dist <= (bursted_tmp->radius + tmp->radius)) {
        tmp->bursted = TRUE;

        new_bursted_tmp = 
          bubblechain_bubble_new_with_color (-j, BUBBLE_BURSTED, tmp->color);

        animate_bursted_bubbles (new_bursted_tmp, tmp->x_c, tmp->y_c);

        bursted_bubbles = g_list_prepend (bursted_bubbles, new_bursted_tmp);
        bubbles = g_list_remove (bubbles, tmp);
        clutter_actor_hide (tmp->actor);
        clutter_container_remove_actor (CLUTTER_CONTAINER (bubbles_group),
                                        tmp->actor);
        update_scoreboard (++bubble_hit);
        g_timeout_add (3500, (GSourceFunc) burst_bubble, new_bursted_tmp);
        break;
      }
    }
  }

  if (bubble_hit == level[current_level]) {
    clutter_actor_animate (stage, CLUTTER_LINEAR, 1200,
                           "color", &color_success,
                           NULL);
  }

  if (g_list_length (bursted_bubbles) == 0 && player_clicked)
    set_next_level ();

  return TRUE;
}

static gboolean
on_button_press (ClutterActor *actor, ClutterEvent *event, gpointer data)
{
  if (!player_clicked) {
    ClutterButtonEvent *ev = (ClutterButtonEvent*)event;
    Bubble *new = 
      bubblechain_bubble_new_with_color (-1, BUBBLE_BURSTED, BUBBLE_BLUE);

    clutter_container_add_actor (CLUTTER_CONTAINER (actor), new->actor);
    bubblechain_bubble_move (new, ev->x, ev->y);

    bursted_bubbles = g_list_prepend (bursted_bubbles, new);
    g_timeout_add (3500, (GSourceFunc) burst_bubble, new);

    player_clicked = TRUE;
    clutter_stage_show_cursor (CLUTTER_STAGE (stage));
    clutter_actor_hide (big_bubble->actor);
  }

  return FALSE;
}

static gboolean
on_motion_event (ClutterActor *actor, ClutterEvent *event, gpointer data)
{
  if (!player_clicked) {
    ClutterMotionEvent *mev = (ClutterMotionEvent *) event;
    clutter_actor_set_position (big_bubble->actor, mev->x, mev->y);
    big_bubble->x = mev->x;
    big_bubble->y = mev->y;
  }
  return FALSE;
}

static void
setup_stage (void) {

  ClutterColor text_color = {0x00,0x88,0xFF,0xff};

  clutter_stage_set_user_resizable (CLUTTER_STAGE (stage), FALSE);
  clutter_stage_set_color (CLUTTER_STAGE (stage), &color_normal);
  clutter_stage_hide_cursor (CLUTTER_STAGE (stage));

  score_label = clutter_text_new_full ("Sans Bold 15", "Score:", &text_color);
  level_label = clutter_text_new_full ("Sans Bold 15", 
                                       "1 bubbles out of 5", &text_color);
  bubbles_count = clutter_text_new_full ("sans Bold 15",
                                        "0", &text_color);
  message_label = clutter_text_new_full ("Sans Bold 15",
                                         "", &text_color);

  clutter_actor_set_position (score_label, 5,3);
  clutter_actor_set_position (level_label, SCREEN_WIDTH / 2 -
                              clutter_actor_get_width (level_label) / 2, 3);
  clutter_actor_set_position (bubbles_count, SCREEN_WIDTH -
                              clutter_actor_get_width (bubbles_count) - 10, 3);
  clutter_actor_set_position (message_label, SCREEN_WIDTH -
                              clutter_actor_get_width (message_label),
                              SCREEN_HEIGHT -
                              clutter_actor_get_height (message_label) * 1.5);

  clutter_container_add (CLUTTER_CONTAINER (stage), 
                         CLUTTER_ACTOR (score_label),
                         CLUTTER_ACTOR (bubbles_count),
                         CLUTTER_ACTOR (level_label),
                         CLUTTER_ACTOR (message_label),
                         NULL);

  load_bubbles (num_bubbles);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), 
                               CLUTTER_ACTOR (bubbles_group));
  
  big_bubble = 
    bubblechain_bubble_new_with_color (1, BUBBLE_BURSTED, BUBBLE_BLUE);
  clutter_actor_set_position (big_bubble->actor, 
                              SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), big_bubble->actor);

  g_signal_connect (stage, "button-press-event", 
                    G_CALLBACK (on_button_press), NULL);
  g_signal_connect (stage, "motion-event", 
                    G_CALLBACK (on_motion_event), big_bubble);
}

int
main (gint argc, gchar **argv)
{
  gtk_clutter_init (&argc, &argv);

  GtkWidget *window;
  GtkWidget *clutter_widget;

  bubbles_group = clutter_group_new ();

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "BubbleChain");
  gtk_window_set_default_size (GTK_WINDOW (window), SCREEN_WIDTH, SCREEN_HEIGHT);
  gtk_widget_set_size_request (window, SCREEN_WIDTH, SCREEN_HEIGHT);
  gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

  clutter_widget = gtk_clutter_embed_new ();
  gtk_container_add (GTK_CONTAINER (window), clutter_widget);
  stage = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter_widget));

  setup_stage ();
  main_id = g_timeout_add (20, (GSourceFunc) main_loop, NULL);
  g_signal_connect (window, "hide", G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_show_all (window);
  gtk_main ();

  return 1;
}
