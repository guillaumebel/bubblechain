#include <clutter/clutter.h>
#include <cairo.h>
#include <math.h>
#include <time.h>

#include "bubble.h"

static void
create_bubble (Bubble* bubble) 
{
  ClutterActor *actor;
  gint x,y;
  
  actor = clutter_cairo_texture_new (bubble->radius*2, bubble->radius*2);
  srand (time (NULL) + bubble->number);
  x = rand () % 640;
  srand (time (NULL) + bubble->number);
  y = rand () % 480;

  cairo_t *cr;
  cairo_pattern_t *pattern;
  cr = clutter_cairo_texture_create (CLUTTER_CAIRO_TEXTURE (actor));
  
  cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint (cr);
  cairo_set_operator (cr, CAIRO_OPERATOR_ADD);
  
  cairo_arc (cr, bubble->radius, bubble->radius, bubble->radius, 0.0, 2*M_PI);
  
  pattern = cairo_pattern_create_radial (bubble->radius, bubble->radius, 0,
                                         bubble->radius, bubble->radius, 
                                         bubble->radius);
  switch (bubble->color){
    case BUBBLE_GREEN:
      cairo_pattern_add_color_stop_rgba (pattern, 0, 0.88, 0.99, 0.95, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.6, 0.88, 0.99, 0.95, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.8, 0.67, 0.91, 0.83, 0.2);
      cairo_pattern_add_color_stop_rgba (pattern, 0.9, 0.5, 0.88, 0.67, 0.7);
      cairo_pattern_add_color_stop_rgba (pattern, 1.0, 0.3, 0.69, 0.43, 0.8);
      break;
    case BUBBLE_PURPLE:
      cairo_pattern_add_color_stop_rgba (pattern, 0, 0.95, 0.88, 0.99, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.6, 0.95, 0.88, 0.99, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.8, 0.83, 0.67, 0.91, 0.2);
      cairo_pattern_add_color_stop_rgba (pattern, 0.9, 0.67, 0.5, 0.88, 0.7);
      cairo_pattern_add_color_stop_rgba (pattern, 1.0, 0.43, 0.3, 0.69, 0.8);
    break;
    case BUBBLE_RED:
      cairo_pattern_add_color_stop_rgba (pattern, 0, 0.99, 0.88, 0.95, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.6, 0.99, 0.88, 0.95, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.8, 0.94, 0.47, 0.43, 0.2);
      cairo_pattern_add_color_stop_rgba (pattern, 0.9, 0.99, 0.30, 0.46, 0.7);
      cairo_pattern_add_color_stop_rgba (pattern, 1.0, 0.84, 0.28, 0.36, 0.8);
      break;
    case BUBBLE_YELLOW:
      cairo_pattern_add_color_stop_rgba (pattern, 0, 0.99, 0.95, 0.88, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.6, 0.99, 0.95, 0.88, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.8, 0.91, 0.83, 0.67, 0.2);
      cairo_pattern_add_color_stop_rgba (pattern, 0.9, 0.83, 0.77, 0.4, 0.7);
      cairo_pattern_add_color_stop_rgba (pattern, 1.0, 0.79, 0.63, 0.25, 0.8);
      break;
    case BUBBLE_ORANGE:
      cairo_pattern_add_color_stop_rgba (pattern, 0, 0.99, 0.95, 0.88, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.6, 0.99, 0.95, 0.88, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.8, 0.91, 0.83, 0.67, 0.2);
      cairo_pattern_add_color_stop_rgba (pattern, 0.9, 0.88, 0.67, 0.5, 0.7);
      cairo_pattern_add_color_stop_rgba (pattern, 1.0, 0.69, 0.43, 0.3, 0.8);
    break;
    case BUBBLE_BLUE:
      cairo_pattern_add_color_stop_rgba (pattern, 0, 0.88, 0.95, 0.99, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.6, 0.88, 0.95, 0.99, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.8, 0.67, 0.83, 0.91, 0.2);
      cairo_pattern_add_color_stop_rgba (pattern, 0.9, 0.5, 0.67, 0.88, 0.7);
      cairo_pattern_add_color_stop_rgba (pattern, 1.0, 0.3, 0.43, 0.69, 0.8);
      break;
    default:
      cairo_pattern_add_color_stop_rgba (pattern, 0, 0.88, 0.95, 0.99, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.6, 0.88, 0.95, 0.99, 0.1);
      cairo_pattern_add_color_stop_rgba (pattern, 0.8, 0.67, 0.83, 0.91, 0.2);
      cairo_pattern_add_color_stop_rgba (pattern, 0.9, 0.5, 0.67, 0.88, 0.7);
      cairo_pattern_add_color_stop_rgba (pattern, 1.0, 0.3, 0.43, 0.69, 0.8);
      break;
  }

  cairo_set_source (cr, pattern);
  cairo_fill_preserve (cr);
  
  cairo_pattern_destroy (pattern);
  
  pattern = cairo_pattern_create_linear (0, 0, 
                                         bubble->radius*2, bubble->radius*2);
  cairo_pattern_add_color_stop_rgba (pattern, 0.0, 1.0, 1.0, 1.0, 0.0);
  cairo_pattern_add_color_stop_rgba (pattern, 0.15, 1.0, 1.0, 1.0, 0.95);
  cairo_pattern_add_color_stop_rgba (pattern, 0.3, 1.0, 1.0, 1.0, 0.0);
  cairo_pattern_add_color_stop_rgba (pattern, 0.7, 1.0, 1.0, 1.0, 0.0);
  cairo_pattern_add_color_stop_rgba (pattern, 0.85, 1.0, 1.0, 1.0, 0.95);
  cairo_pattern_add_color_stop_rgba (pattern, 1.0, 1.0, 1.0, 1.0, 0.0);

  cairo_set_source (cr, pattern);
  cairo_fill (cr);
  
  cairo_pattern_destroy (pattern); 
  cairo_destroy (cr);

  clutter_actor_set_position (CLUTTER_ACTOR (actor), (gfloat)x, (gfloat)y);

  bubble->actor = actor;
}

Bubble*
bubblechain_bubble_new (gint num, gint type)
{
  Bubble *bubble = g_new (Bubble, 1);
  bubble->x = 0.0;
  bubble->y = 0.0;
  bubble->x_c = 0.0;
  bubble->y_c = 0.0;
  bubble->number = num;

  if (type == BUBBLE_BURSTED) {
    bubble->radius = BUBBLE_R * 6;
    bubble->bursted = TRUE;
  } else {
    bubble->radius = BUBBLE_R;
    bubble->bursted = FALSE;
  }

  bubble->color = rand() % 6 + 1;
  create_bubble (bubble);
  clutter_actor_get_position (bubble->actor, &bubble->x, &bubble->y);

  srand (time (NULL) + num);
  float dir = ((float) (rand () % 4500) / 100) * (M_PI / 360);
  if (bubble->number % 2 != 0) {
    bubble->hspeed = 2 * sin (dir);
    bubble->vspeed = 2 * cos (dir);
  } else {
    bubble->hspeed = 2 * cos (dir);
    bubble->vspeed = 2 * sin (dir);
  }

  return bubble;
}

Bubble*
bubblechain_bubble_new_with_color (gint num, gint type, gint color)
{
  Bubble *bubble = g_new (Bubble, 1);
  bubble->x = 0.0;
  bubble->y = 0.0;
  bubble->x_c = 0.0;
  bubble->y_c = 0.0;
  bubble->number = num;

  if (type == BUBBLE_BURSTED) {
    bubble->radius = BUBBLE_R * 6;
    bubble->bursted = TRUE;
  } else {
    bubble->radius = BUBBLE_R;
    bubble->bursted = FALSE;
  }

  bubble->color = color;
  create_bubble (bubble);
  clutter_actor_get_position (bubble->actor, &bubble->x, &bubble->y);

  srand (time (NULL) + num);
  float dir = ((float) (rand () % 4500) / 100) * (M_PI / 360);
  if (bubble->number % 2 != 0) {
    bubble->hspeed = 2 * sin (dir);
    bubble->vspeed = 2 * cos (dir);
  } else {
    bubble->hspeed = 2 * cos (dir);
    bubble->vspeed = 2 * sin (dir);
  }

  return bubble;
}

void
bubblechain_bubble_move (Bubble *bubble, gfloat x, gfloat y)
{
  clutter_actor_set_position (bubble->actor, x, y);
  bubble->x_c = x + bubble->radius;
  bubble->y_c = y + bubble->radius;
}
