#include "core.h"

#define NINFOSEND_DELAY 4
#define SRMQUEUE_SIZE   1

void sram_init (sram* srm) {

    memset ((void*)&(srm->timer), 0, sizeof(sram_t));
    memset ((void*)&(srm->ninfo), 0, sizeof(neuron_info) * NEURONS);
    queue_init (&(srm->rq), SRMQUEUE_SIZE);
    
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
    (tkn->state) += 1;
    return 0;
}

void sram_advance (core* mycore) {
    send_ninfo_to_token (mycore);
}