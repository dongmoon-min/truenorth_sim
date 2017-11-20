#include "core.h"

void chip_init (chip* mychip, char* ch) {
    
    core* mycore;
    int i;

    for (i = 0; i < CHIP_LENGTH*CHIP_LENGTH; i++) {

        mycore = &(mychip->cores[i]);

        router_init (&(mycore->rtr));
        scheduler_init (&(mycore->sch));
        token_init (&(mycore->tkn));
        sram_init (&(mycore->srm), ch);
        neuron_init (mycore);
    }

    return;
}

void chip_advance (chip* mychip, int gclk) {

    core* mycore;
    int i;

    for (i = 0; i < CHIP_LENGTH*CHIP_LENGTH; i++) {
           
        mycore = &(mychip->cores[i]);

        router_advance (mychip, i, gclk);
        scheduler_advance (mycore);
        token_advance (mycore, gclk);
        sram_advance (mycore);
        neuron_advance (mycore, i);
    }

    return;
}