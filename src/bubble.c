#include <clutter/clutter.h>
#include <cairo.h>
#include <math.h>

#include "bubble.h"

Bubble*
bubblechain_bubble_new (gint num)
{
  Bubble *bubble = g_new (Bubble*, 1);
  bubble->number = num;
  bubble->radius = BUBBLE_R;
  bubble->actor = clutter_cairo_texture_new (BUBBLE_R*2, BUBBLE_R*2);
  bubble->x = rand () % SCREEN_WIDTH - BUBBLE_R;
  bubble->y = rand () % SCREEN_HEIGHT - BUBBLE_R;

  cairo_t *cr;
  cairo_pattern_t *pattern;
  cr = clutter_cairo_texture_create (CLUTTER_CAIRO_TEXTURE (bubble->actor));
  
  cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint (cr);
  cairo_set_operator (cr, CAIRO_OPERATOR_ADD);
  
  cairo_arc (cr, BUBBLE_R, BUBBLE_R, BUBBLE_R, 0.0, 2*M_PI);
  
  pattern = cairo_pattern_create_radial (BUBBLE_R, BUBBLE_R, 0,
                                         BUBBLE_R, BUBBLE_R, BUBBLE_R);
  cairo_pattern_add_color_stop_rgba (pattern, 0, 0.88, 0.95, 0.99, 0.1);
  cairo_pattern_add_color_stop_rgba (pattern, 0.6, 0.88, 0.95, 0.99, 0.1);
  cairo_pattern_add_color_stop_rgba (pattern, 0.8, 0.67, 0.83, 0.91, 0.2);
  cairo_pattern_add_color_stop_rgba (pattern, 0.9, 0.5, 0.67, 0.88, 0.7);
  cairo_pattern_add_color_stop_rgba (pattern, 1.0, 0.3, 0.43, 0.69, 0.8);
  
  cairo_set_source (cr, pattern);
  cairo_fill_preserve (cr);
  
  cairo_pattern_destroy (pattern);
  
  pattern = cairo_pattern_create_linear (0, 0, BUBBLE_R*2, BUBBLE_R*2);
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

  clutter_actor_set_position (CLUTTER_ACTOR (bubble->actor),
                              bubble->x, bubble->y);
  srand (time (NULL));
  float dir = ((float) (rand () % 4500) / 100) * (M_PI / 360);
  if (bubble->number % 2 != 0) {
    bubble->hspeed = 3 * sin (dir);
    bubble->vspeed = 3 * cos (dir);
  } else {
    bubble->hspeed = 3 * cos (dir);
    bubble->vspeed = 3 * sin (dir);
  }

  return bubble;
}
