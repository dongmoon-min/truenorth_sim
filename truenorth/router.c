#include "core.h"

// #define ROUTERQUEUE_SIZE    4 *defined in "core.h"
#define PACKETSEND_DELAY    5
#define SPIKESEND_DELAY     2

int router_drops = 0;  // abandoned packets

static int rtr_checker = 0; // variable for preventing dual activation checking of router

void router_init (router* myrouter) {
    
    // init timers
    myrouter->timer.rs_timer = 0;
    myrouter->timer.rrl_timer = 0;
    myrouter->timer.rrr_timer = 0;
    myrouter->timer.rru_timer = 0;
    myrouter->timer.rrd_timer = 0;

    // init queue;
    queue_init (&(myrouter->leftq), ROUTERQUEUE_SIZE);
    queue_init (&(myrouter->rightq), ROUTERQUEUE_SIZE);
    queue_init (&(myrouter->upperq), ROUTERQUEUE_SIZE);
    queue_init (&(myrouter->downq), ROUTERQUEUE_SIZE);
    queue_init (&(myrouter->inq), ROUTERQUEUE_SIZE);

    myrouter->router_activate = 0;

    return;
}

// recieve a packet and insert the packet in an appropriate queue
int recieve_packet (router* des, packet* pkt) {
    
    // insert packet in inner-queue, queue for save massage send to scheduler
    if (pkt->dx == 0 && pkt->dy == 0) {
        return enqueue (&(des->inq), (void*)pkt);
    }
    // insert packet in right-queue, queue for save massage send to right side router
    if (pkt->dx > 0) {
        pkt->dx--;
        return enqueue (&(des->rightq), (void*)pkt);
    }
    // insert packet in left-queue, queue for save massage send to left side router
    if (pkt->dx < 0) {
        pkt->dx++;
        return enqueue (&(des->leftq), (void*)pkt);
    }
    // insert packet in upper-queue, queue for save massage send to upper side router
    if (pkt->dy < 0) {
        pkt->dy++;
        return enqueue (&(des->upperq), (void*)pkt);
    }
    // insert packet in down-queue, queue for save massage send to down side router
    if (pkt->dy > 0) {
        pkt->dy--;
        return enqueue (&(des->downq), (void*)pkt);
    }
    return -1;
}

// send a packet, existed in 'myqueue', to another router
int send_packet_rtr_to_rtr (router* des, router* src, queue* myqueue, int* timer) {
    
    packet* ptr;

    // in case queue is empty
    if (isempty (myqueue)) {
        return 0;
    }
    if (!rtr_checker) {
        src->router_activate++;
        rtr_checker = 1;
    }
    // waiting start
    if (*timer == 0) {
        *timer = PACKETSEND_DELAY;
        return 0;
    }
    (*timer) -= 1;
    // if router have to wait more time, return
    if (*timer != 0) {
        return 0;
    }
    // send packet to destination router
    ptr = (packet*) dequeue (myqueue);
    if (recieve_packet (des, ptr) < 0) {
        
        // when you want to drop your packet if target queue is full,
        //printf ("packet queue is full!\n");
        //free ((void*)ptr); 
        //drops++;

        // when you resume sending your packet later,
        enqueue (myqueue, (void*)ptr);
        return -1;
    }
    return 0;
}

// send a packet, existed in 'myqueue', to scheduler
int send_packet_to_scheduler (scheduler* des, queue* myqueue, int* timer) {
    
    packet* pkt;
    spike_info* ptr;

    // if queue is empty, return
    if (isempty (myqueue)) {
        return 0;
    }
    // waiting start
    if (*timer == 0) {
        *timer = SPIKESEND_DELAY;
        return 0;
    }
    (*timer) -= 1;
    // if router have to wait more time, return
    if (*timer != 0) {
        return 0;
    }
    // send packet to local scheduler
    pkt = (packet*) dequeue (myqueue);
    ptr = (spike_info*) malloc (sizeof(spike_info));
    memcpy ((void*)ptr, (void*)&(pkt->spk), sizeof(spike_info));
    free (pkt);
    if (enqueue (&(des->rq), (void*)ptr) < 0) {
        printf ("router-scheduler queue is full!\n");
        return -1;
    }
    return 0;
}

void packet_drop (core* mycore, int tick) {
    
    //int* packet_drops;
    router* rtr;

    if (tick % GTICK_INTERVAL != 0)
        return;
    // before initiate new global synchronous tick, flushout all router queue
    //packet_drops = &(mycore->packet_drops);
    //packet_drops = &drops;
    rtr = &(mycore->rtr);
    router_drops += queue_flush (&(rtr->leftq));
    router_drops += queue_flush (&(rtr->rightq));
    router_drops += queue_flush (&(rtr->upperq));
    router_drops += queue_flush (&(rtr->downq));

    return;
}

void router_advance (chip* mychip, int core_no, int tick) {

    core* cur_core = &(mychip->cores[core_no]);
    router* myrouter = &(cur_core->rtr);

    send_packet_to_scheduler (&(cur_core->sch), &(myrouter->inq), &(myrouter->timer.rs_timer));
    packet_drop (cur_core, tick);
    
    rtr_checker = 0;
    // left
    if (core_no % CHIP_LENGTH != 0)
        send_packet_rtr_to_rtr (&(mychip->cores[core_no - 1].rtr), myrouter, &(myrouter->leftq), &(myrouter->timer.rrl_timer));
    // right
    if (core_no % CHIP_LENGTH != CHIP_LENGTH - 1)
        send_packet_rtr_to_rtr (&(mychip->cores[core_no + 1].rtr), myrouter, &(myrouter->rightq), &(myrouter->timer.rrr_timer));
    // down
    if (core_no / CHIP_LENGTH != CHIP_LENGTH - 1)
        send_packet_rtr_to_rtr (&(mychip->cores[core_no + CHIP_LENGTH].rtr), myrouter, &(myrouter->downq), &(myrouter->timer.rrd_timer));
    // upper
    if (core_no / CHIP_LENGTH != 0)
        send_packet_rtr_to_rtr (&(mychip->cores[core_no - CHIP_LENGTH].rtr), myrouter, &(myrouter->upperq), &(myrouter->timer.rru_timer));

    return;
}