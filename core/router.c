#include "core.h"

#define ROUTERQUEUE_SIZE    40
#define PACKETSEND_DELAY    2
#define SPIKESEND_DELAY     2

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
    if (pkt->dy > 0) {
        pkt->dy--;
        return enqueue (&(des->upperq), (void*)pkt);
    }
    // insert packet in down-queue, queue for save massage send to down side router
    if (pkt->dy < 0) {
        pkt->dy++;
        return enqueue (&(des->downq), (void*)pkt);
    }
    return -1;
}

// send a packet, existed in 'myqueue', to another router
int send_packet_rtr_to_rtr (router* des, queue* myqueue, int* timer) {
    
    packet* ptr;

    // in case queue is empty
    if (isempty (myqueue)) {
        return 0;
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
        printf ("packet queue is full!\n");
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

void router_advance (chip* mychip, int core_no) {

    core* cur_core = &(mychip->cores[core_no]);
    router* myrouter = &(cur_core->rtr);

    send_packet_to_scheduler (&(cur_core->sch), &(myrouter->inq), &(myrouter->timer.rs_timer));
    
    // left
    if (core_no % CHIP_LENGTH != 0)
        send_packet_rtr_to_rtr (&(mychip->cores[core_no - 1].rtr), &(myrouter->leftq), &(myrouter->timer.rrl_timer));
    // right
    if (core_no % CHIP_LENGTH != CHIP_LENGTH - 1)
        send_packet_rtr_to_rtr (&(mychip->cores[core_no + 1].rtr), &(myrouter->rightq), &(myrouter->timer.rrr_timer));
    // down
    if (core_no / CHIP_LENGTH != CHIP_LENGTH - 1)
        send_packet_rtr_to_rtr (&(mychip->cores[core_no + CHIP_LENGTH].rtr), &(myrouter->downq), &(myrouter->timer.rrd_timer));
    // upper
    if (core_no / CHIP_LENGTH != 0)
        send_packet_rtr_to_rtr (&(mychip->cores[core_no - CHIP_LENGTH].rtr), &(myrouter->upperq), &(myrouter->timer.rru_timer));

    return;
}