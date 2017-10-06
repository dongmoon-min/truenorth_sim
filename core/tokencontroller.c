#include "core.h"

#define TKNQUEUE_SIZE       256
#define TKNCOMPUTE_DELAY    2   

void token_init (token* mytoken) {

    memset ((void*)&(mytoken->timer), 0, sizeof(token_t));
    mytoken->input = NULL;
    mytoken->ninfo = NULL;
    mytoken->state = 2*NEURONS;
    queue_init (&(mytoken->rq), TKNQUEUE_SIZE);
    
    return;
}

int send_request_to_scheduler (core* mycore, int tick) {

    scheduler* sch = &(mycore->sch);
    sch_request* rqst = (sch_request*) malloc (sizeof(sch_request));
    rqst->tick = tick;
    
    return enqueue (&(sch->tq), (void*)rqst);
}

int send_request_to_sram (core* mycore, int neuron_num) {

    sram* srm = &(mycore->srm);
    s_request* rqst = (s_request*) malloc (sizeof(s_request));
    rqst->neuron_num = neuron_num;

    return enqueue (&(srm->rq), (void*)rqst);
}

int send_request_to_neuron (core* mycore, compute_info* cinfo) {

    neuron* nrn = &(mycore->nrn);

    return enqueue (&(nrn->crq), (void*)cinfo);
}

// advancing request processing unit('token request block') in TokenController
// what a state mean,
// state/2 = target neuron, 
// state%2 = add: need to send request to scheduler & sram, odd: waiting axon, neuron value from scheduler & sram
int token_request_block (core* mycore, int gclk) {

    token* tkn = &(mycore->tkn);
    int tick = (gclk / GTICK_INTERVAL) % TICK_NUMBER;
    int* state = &(tkn->state);
    int neuron_num = (*state)/2;
    compute_info* cinfo = NULL;

    // processing start
    if (gclk % GTICK_INTERVAL == 0)
        *state = 0;
    // processing end, need new global synchronous clk tick
    if (*state == 2 * NEURONS)
        return 0;
    // if block need to send a request to sch & sram,
    if ((*state) % 2 == 0) {
        send_request_to_scheduler (mycore, tick);
        send_request_to_sram (mycore, neuron_num);
    }
    // else block need to receive axon, neuron_info and send it to TokenComputeBlock,
    else {
        if (tkn->input == NULL || tkn->ninfo == NULL)
            return 0;
        cinfo = (compute_info*) malloc (sizeof(compute_info));
        cinfo->neuron_no = neuron_num;
        memcpy ((void*)&(cinfo->ninfo), (void*)tkn->ninfo, sizeof(neuron_info));
        memcpy ((void*)&(cinfo->spike), (void*)tkn->input, sizeof(axon));
        free ((void*)tkn->ninfo);
        free ((void*)tkn->input);
        tkn->ninfo = NULL;
        tkn->input = NULL;
        enqueue (&(tkn->rq), (void*)cinfo);
    }
    (*state) += 1;
    return 0;
}

// advancing comparing unit ('token compute unit') in TokenController
int token_compute_block (core* mycore) {

    token* tkn = &(mycore->tkn);
    queue* rq = &(tkn->rq);
    int* timer = &(tkn->timer.tn_timer);
    int i;
    compute_info* cinfo = NULL;
    int* synapse;
    int* spike;

    // if there is no request from request block, return
    if (isempty (rq)) {
        return 0;
    }
    // waiting start
    if (*timer == 0) {
        *timer = TKNCOMPUTE_DELAY;
        return 0;
    }
    (*timer) -= 1;
    // if token compute block need more waiting time, return
    if (*timer != 0) {
        return 0;
    }
    // comparing to setting iscompute value, and send compute_info
    cinfo = (compute_info*) dequeue (rq);
    cinfo->iscompute = 0;
    synapse = cinfo->ninfo.synapse;
    spike = cinfo->spike.spike;
    for (i = 0; i < AXON_NUMBER; i++) {
        if (synapse[i] == 1 && spike[i] == 1) {
            cinfo->iscompute = 1;
            break;
        }
    }
    return send_request_to_neuron (mycore, cinfo);
}

void token_advance (core* mycore, int gclk) {

    token_request_block (mycore, gclk);
    token_compute_block (mycore);

    return;
}