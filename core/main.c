#include "core.h"

#define SIMTIME     10000
#define FILE_NAME   256

chip mychip;

void help ();

int main (int argc, char* argv[]) {

    int opt, i;
    char filename[FILE_NAME];
    memcpy (filename, "data/random_data", 256);

    // -h: help, -g: GUI ON, -f: filename
    while ((opt = getopt (argc, argv, "hgf:")) != -1) {
        switch (opt) {
            case 'h':
                help ();
                break;
            case 'g':
                break;
            case 'f':
                memcpy (filename, optarg, 256);
                break;
        }
    }

    // initiate TrueNorth chip
    printf ("now initiate a chip...\n");
    chip_init (&mychip, filename);
    printf ("complete!\n");

    // simulate TrueNorth for 'SIMTIME' tick
    printf ("simulate TrueNorth Chip for %dms...\n", SIMTIME/GTICK_INTERVAL);
    for (i = 0; i < SIMTIME; i++) {
        chip_advance (&mychip, i);
        if (i%GTICK_INTERVAL == 0)
            printf ("global clock: %d\n", i/GTICK_INTERVAL);
    }
    printf ("complete!\n");

    return 0;
}


void help () {

    printf ("Usage: ./truenorth [OPTION] [FILE]\n");
    printf ("-h\t\thelp\n");
    printf ("-g\t\tGUI ON\n");
    printf ("-f [FILE]\tsync neuron_info data\n");

    exit (0);
}