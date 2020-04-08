#ifndef __DRAW_OPTS_H
#define __DRAW_OPTS_H

#define DRAW_OPTS_COLOR_DEFAULT 0
#define DRAW_OPTS_COLOR_RAND 1

typedef struct {
    const int color = DRAW_OPTS_COLOR_DEFAULT;
    const float t = 0;
} draw_opts;

#endif