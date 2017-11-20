#include "core.h"

#define NINFOSEND_DELAY 5
#define SRMQUEUE_SIZE   1

#define WEIGHT_RANGE    60
#define LEAK_RANGE      4
#define SPIKE_RATE      10

void make_neuron_info (sram* srm, char* ch);

void sram_init (sram* srm, char* ch) {

    memset ((void*)&(srm->timer), 0, sizeof(sram_t));
    memset ((void*)&(srm->ninfo), 0, sizeof(neuron_info) * NEURONS);
    queue_init (&(srm->rq), SRMQUEUE_SIZE);
    srm->sram_activate = 0;

    make_neuron_info (srm, ch);
    
    return;
}

int send_ninfo_to_token (core* mycore) {

    token* tkn = &(mycore->tkn);
    sram* srm = &(mycore->srm);
    queue* myqueue = &(srm->rq);
    int* timer = &(srm->timer.st_timer);
    s_request* rqst;
    neuron_info* ninfo;

    // if there is no request from token, return
    if (isempty (&(srm->rq))) {
        return 0;
    }
    srm->sram_activate++;
    //waiting start
    if (*timer == 0) {
        *timer = NINFOSEND_DELAY;
        return 0;
    }
    (*timer) -= 1;
    // if SRAM have to wait more time, return
    if (*timer != 0) {
        return 0;
    }
    // send neuron_info to TokenController
    rqst = (s_request*) dequeue (myqueue);
    ninfo = (neuron_info*) malloc (sizeof(neuron_info));
    memcpy ((void*)ninfo, (void*)&(srm->ninfo[rqst->neuron_num]), sizeof(neuron_info));
    free (rqst);
    if (tkn->ninfo != NULL) {
        return -1;
    }
    tkn->ninfo = ninfo;
    //(tkn->state) += 1;
    return 0;
}


void sram_advance (core* mycore) {
    send_ninfo_to_token (mycore);
}

void make_neuron_info (sram* srm, char* ch) {
    
    int i, j;
    
    // get neuron data from the file
    for (i = 0; i < NEURONS; i++) {
        srand (time(NULL)+rand());
        // get synapse info
        for (j = 0; j < AXON_NUMBER; j++) {
            srm->ninfo[i].synapse[j] = (rand () % 2);
        }
        // get weight info
        for (j = 0; j < AXON_NUMBER; j++) {
            srm->ninfo[i].weight[j] = (rand () % WEIGHT_RANGE);
        }
        // get leak info
        srm->ninfo[i].leak = (rand () % LEAK_RANGE);
        // get destination info (i,e, dest, des_axon, tick)
        srm->ninfo[i].dest = (rand () % (CHIP_LENGTH*CHIP_LENGTH));
        srm->ninfo[i].des_axon = (rand () % AXON_NUMBER);
        srm->ninfo[i].tick = (rand () % TICK_NUMBER);
        // get nopt info (type of neuron)
        srm->ninfo[i].nopt = (rand () % SPIKE_RATE);
    }

    return;
}