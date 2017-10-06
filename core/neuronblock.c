#include "core.h"

#define NRNQUEUE_SIZE       256
#define NRNCOMPUTE_DELAY    10
#define NRNPSEND_DELAY      5
#define NRNNINFO_DELAY      5

#define THRESHOLD_VOLT      100
#define BOTTOM_VOLT         0


void neuron_init (core* mycore) {

    neuron* nrn = &(mycore->nrn);
    memset(&(nrn->timer), 0, sizeof(token_t));
    queue_init (&(nrn->crq), NRNQUEUE_SIZE);
    queue_init (&(nrn->prq), NRNQUEUE_SIZE);
    queue_init (&(nrn->nrq), NRNQUEUE_SIZE);

    return;
}

void dxdy_compute (int coreno, int des, int* dx, int* dy) {
    *dx = (des/CHIP_LENGTH) - (coreno/CHIP_LENGTH);
    *dy = (des%CHIP_LENGTH) - (coreno%CHIP_LENGTH);
    return;
}

int neuron_compute (core* mycore, int coreno) {

    neuron* nrn = &(mycore->nrn);
    int* timer = &(nrn->timer.in_timer);
    compute_info* cinfo = NULL;
    queue* crq = &(nrn->crq);
    queue* prq = &(nrn->prq);
    queue* nrq = &(nrn->nrq);
    int i;
    packet* pkt = NULL;

    // if queue is empty, return
    if (isempty (crq)) {
        return 0;
    }
    // waiting start
    if (*timer == 0) {
        *timer = NRNCOMPUTE_DELAY;
        return 0;
    }
    (*timer) -= 1;
    // if compute block have to wait more time, return
    if (*timer != 0) {
        return 0;
    }

    cinfo = (compute_info*) dequeue (crq);
    // when input spike location and synapse is equal, compute
    if (cinfo->iscompute == 1) {
        for (i = 0; i < NEURONS; i++) {
            cinfo->ninfo.potential += cinfo->ninfo.weight[i] * cinfo->ninfo.synapse[i] * cinfo->spike.spike[i];
        }
        // if potential is over the threshold voltage, send a packet to router
        if (cinfo->ninfo.potential >= THRESHOLD_VOLT) {
            cinfo->ninfo.potential = BOTTOM_VOLT;
            pkt = (packet*) malloc (sizeof(packet));
            dxdy_compute (coreno, cinfo->ninfo.dest, &(pkt->dx), &(pkt->dy));
            pkt->spk.axonno = cinfo->ninfo.des_axon;
            pkt->spk.tick = cinfo->ninfo.tick;
            enqueue (prq, (void*)pkt);
        }
    }
    // if potential is lower than bottom voltage,
    cinfo->ninfo.potential -= cinfo->ninfo.leak;
    if (cinfo->ninfo.potential < BOTTOM_VOLT) {
        cinfo->ninfo.potential = BOTTOM_VOLT;
    }
    // send updated neuron_info to nrq
    enqueue (nrq, (void*)cinfo);
    return 0;
}

int send_packet_nrn_to_rtr (core* mycore) {

    neuron* nrn = &(mycore->nrn);
    int* timer = &(nrn->timer.nr_timer);
    queue* prq = &(nrn->prq);
    packet* pkt = NULL;

    // if queue is empty, return
    if (isempty (prq)) {
        return 0;
    }
    // waiting start
    if (*timer == 0) {
        *timer = NRNPSEND_DELAY;
        return 0;
    }
    (*timer) -= 1;
    // if packet sending block need more time, return;
    if (*timer != 0) {
        return 0;
    }
    // send packet to router
    pkt = (packet*) dequeue (prq);
    recieve_packet (&(mycore->rtr), pkt);
    return 0;
}

int send_ninfo_to_sram (core* mycore) {

    neuron* nrn = &(mycore->nrn);
    int* timer = &(nrn->timer.ns_timer);
    compute_info* cinfo = NULL;
    queue* nrq = &(nrn->nrq);

    // if queue is empty, return
    if (isempty (nrq)) {
        return 0;
    }
    // waiting start
    if (*timer == 0) {
        *timer = NRNNINFO_DELAY;
        return 0;
    }
    (*timer) -= 1;
    // if sram saving block have to wait more time, return
    if (*timer != 0) {
        return 0;
    }

    cinfo = (compute_info*) dequeue (nrq);
    memcpy ((void*)&(mycore->srm.ninfo[cinfo->neuron_no]), (void*)&(cinfo->ninfo), sizeof(neuron_info));
    free ((void*)cinfo);
    return 0;
}

void neuron_advance (core* mycore, int coreno) {

    neuron_compute (mycore, coreno);
    send_packet_nrn_to_rtr (mycore);
    send_ninfo_to_sram (mycore);
    
    return;
}