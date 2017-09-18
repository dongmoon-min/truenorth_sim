#include <stdio.h>

#define ADD_DELAY   1
#define SUB_DELAY   3
#define MAX_TIME    100

typedef struct {
    int value;
    int ischange;
} pathway;

void path_init (pathway* path, int value) {
    path->ischange = 0;
    path->value = value;
}

void timer_init (int* timer) {
    *timer = 0;
}

void add (int* timer, pathway* in1, pathway* in2, pathway* out) {
    if (*timer == 0) {
        if (in1->ischange || in2->ischange)
            *timer = ADD_DELAY;
        return;
    }
    if (*timer > 0) {
        if (*timer == 1) {
            in1->ischange = 0;
            in2->ischange = 0;
            out->value = in1->value + in2->value;
            out->ischange = 1;
        }
        *timer = *timer - 1;
    }
}

void sub (int* timer, pathway* in1, pathway* in2, pathway* out) {
    if (*timer == 0) {
        if (in1->ischange || in2->ischange)
            *timer = SUB_DELAY;
        return;
    }
    if (*timer > 0) {
        if (*timer == 1) {
            in1->ischange = 0;
            in2->ischange = 0;
            out->value = in1->value - in2->value;
            out->ischange = 1;
        }
        *timer = *timer - 1;
    }
}

pathway line1, line2, line3, line4;
int add_timer, sub_timer;

void advance (int tick) {
    add (&add_timer, &line1, &line4, &line2);
    sub (&sub_timer, &line2, &line3, &line4);
    printf ("%dns\tline2:\t%d\tline4:\t%d\n", tick, line2.value, line4.value);
}

int main () {
    int i;

    path_init (&line1, 3);
    path_init (&line2, 0);
    path_init (&line3, 2);
    path_init (&line4, 1);

    timer_init (&add_timer);
    timer_init (&sub_timer);

    line1.ischange = 1;

    for (i = 0; i < MAX_TIME; i++) {
        advance (i);
    }
}