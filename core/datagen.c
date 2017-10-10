#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NEURON_NUMBER   (4096*256)
#define AXON_NUMBER     256
#define WEIGHT_RANGE    5
#define DES_RANGE       4096
#define TICK_RANGE      16
#define LEAK_VAL        10

int main () {
    
    FILE* fd = NULL;
    int i, j, data;

    // create file for containing neuron info
    printf ("now create neuron data file...\n");
    fd = fopen ("data/random_data", "w+");
    srand (time(NULL));
    printf ("complete!\n");

    // generate neuron data and insert it in the file
    printf ("now generate neuron information...(1.1GB)\n");
    for (i = 0; i < NEURON_NUMBER; i++) {
        // generate synapse info
        for (j = 0; j < AXON_NUMBER; j++) {
            data = (rand () % 2);
            fprintf (fd, "%d ", data);
        }
        // generate weight info
        for (j = 0; j < AXON_NUMBER; j++) {
            data = (rand () % WEIGHT_RANGE);
            fprintf (fd, "%d ", data);
        }
        // generate leak info
        data = LEAK_VAL; //data = (rand () % 100);
        fprintf (fd, "%d ", data);
        // generate destination info (i,e, dest, des_axon, tick)
        data = (rand () % DES_RANGE);
        fprintf (fd, "%d ", data);
        data = (rand () % AXON_NUMBER);
        fprintf (fd, "%d ", data);
        data = (rand () % TICK_RANGE);
        fprintf (fd, "%d\n", data);
    }
    printf ("complete!\n\n");
    printf ("data location: data/random_data\n");

    return 0;
}