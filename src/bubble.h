#ifndef BUBBLE_H_
#define BUBBLE_H_

#include <clutter/clutter.h>

#define SCREEN_WIDTH 680
#define SCREEN_HEIGHT 480
#define BUBBLE_R 8

#define BUBBLE_NORMAL 0
#define BUBBLE_BURSTED 1

typedef struct Bubble {
  gint number;
  ClutterActor *actor;
  gint radius;
  gfloat x, y;
  gfloat x_c, y_c;
  gfloat vspeed, hspeed;
  gboolean bursted;
} Bubble;

Bubble* bubblechain_bubble_new (gint num, gint type);
void bubblechain_bubble_move (Bubble* bubble, gfloat x, gfloat y);
#endif
