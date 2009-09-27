#ifndef BUBBLE_H_
#define BUBBLE_H_

#include <clutter/clutter.h>

#define SCREEN_WIDTH 680
#define SCREEN_HEIGHT 480
#define BUBBLE_R 12

typedef struct Bubble {
  gint number;
  ClutterActor *actor;
  gint radius;
  gfloat x, y;
  gfloat vspeed, hspeed;
} Bubble;

Bubble* bubblechain_bubble_new (gint num);

#endif
