#include "core.h"

#define SIMTIME 10000

chip mychip;

int main () {
    int i;
    chip_init (&mychip);
    for (i = 0; i < SIMTIME; i++) {
        chip_advance (&mychip, i);
        if (i%GTICK_INTERVAL == 0)
            printf ("global clock: %d\n", i/GTICK_INTERVAL);
    }
    return 0;
}