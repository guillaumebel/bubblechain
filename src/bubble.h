#ifndef BUBBLE_H_
#define BUBBLE_H_

#include <clutter/clutter.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BUBBLE_R 8

#define BUBBLE_NORMAL 0
#define BUBBLE_BURSTED 1

#define BUBBLE_BLUE 1
#define BUBBLE_PURPLE 2
#define BUBBLE_RED 3
#define BUBBLE_GREEN 4
#define BUBBLE_YELLOW 5
#define BUBBLE_ORANGE 6

typedef struct Bubble {
  gint number;
  ClutterActor *actor;
  gint radius;
  gfloat x, y;
  gfloat x_c, y_c;
  gfloat vspeed, hspeed;
  gboolean bursted;
  gint color;
} Bubble;

Bubble* bubblechain_bubble_new (gint num, gint type);
Bubble* bubblechain_bubble_new_with_color (gint num, gint type, gint color);
void bubblechain_bubble_move (Bubble* bubble, gfloat x, gfloat y);
#endif
