#ifndef BUBBLE_H_
#define BUBBLE_H_

#include <clutter/clutter.h>

typedef struct Bubble {
  ClutterActor *actor;
  gint radius;
  gint x, y;
  gint vspeed, hspeed;
} Bubble;

Bubble* bubblechain_bubble_new (void);

#endif
