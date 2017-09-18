#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <string.h>
#include "queue.h"

#define NEURONS         256
#define AXON_NUMBER     256
#define CHIP_LENGTH     64
#define TICK_NUMBER     16

/********************************************************************************/
/******************************** timer elements ********************************/
/********************************************************************************/

/* timer objects of core elements */
typedef struct {
    int st_timer;   // scheduler-tokenController communication
} scheduler_t;

typedef struct {
    int tn_timer;   // token-NeuronBlock communication
    int in_timer;   // inner-processing (comparing spike & synapse)
} token_t;

typedef struct {
    int st_timer;   // sram-tokenController communication
} sram_t;

typedef struct {
    int ns_timer;   // neuronBlock-sram communication
    int nr_timer;   // neuronBlock-router communication
    int in_timer;   // inner processing (computation & leak & threshold check)
} neuron_t;

typedef struct {
    int rs_timer;   // router-scheduler communication
    int rrl_timer;   // router-router(left) communication
    int rrr_timer;   // router-router(right) communication
    int rru_timer;   // router-router(upper) communication
    int rrd_timer;   // router-router(down) communication
} router_t;


/********************************************************************************/
/******************************* message elements *******************************/
/********************************************************************************/

// router to scheduler
typedef struct {
    int axonno;
    int tick;
} spike_info;

// router to router, NeuronBlock to Router
typedef struct {
    int dx;
    int dy;
    spike_info spk;
} packet;

//scheduler to TokenController
typedef struct {
    int spike[AXON_NUMBER];
} axon;

// CoreSRAM to TokenController, CoreSRAM to NeuronBlock, NeuronBlock to CoreSRAM
typedef struct {
    int synapse[AXON_NUMBER];
    int weight[AXON_NUMBER];
    int leak;
    int potential;
    int dest;
    int tick;
} neuron_info;

// TokenController to NeuronBlock
typedef struct {
    int neuron_no;
    int iscompute;
    neuron_info* ninfo;
    axon* spike;
} compute_info;

// request from TokenController to Scheduler
typedef struct {
    int tick;
} sch_request;

// request from TokenController to CoreSRAM
typedef struct {
    int neuron_num;
} s_request;


/********************************************************************************/
/********************************* data elements ********************************/
/********************************************************************************/

typedef struct {
    router_t timer;
    queue leftq;
    queue rightq;
    queue upperq;
    queue downq;
    queue inq;
} router

typedef struct {
    scheduler_t timer;
    axons[TICK_NUMBER];   // local SRAM to save spike data
    queue rq    // save request from router
    queue tq    // save request from TokenController
} scheduler

typedef struct {
    token_t timer;
    axon* input;
    neuron_info* ninfo;
    int state;
} token;

typedef struct {
    neuron_t timer;
    queue* crq   // save computation request from TokenController
    compute_info* cinfo;
    int state;
} neuron;

typedef struct {
    sram_t timer;
    s_request* rqst;
} sram;

typedef struct {
    router rtr;
    scheduler sch;
    token tkn;
    neuron nrn;
    sram srm;
} core;

typedef struct {
    core cores[CHIP_LENGTH * CHIP_LENGTH];
} chip;


/********************************************************************************/
/********************************** functions ***********************************/
/********************************************************************************/

/* router functions */
void router_init (router* myrouter);
int recieve_packet (router* des, packet* pkt);
void router_advance (chip* mychip, int core_no);

/* scheduler functions */
void scheduler_init (scheduler* sch);
void scheduler_advance (core* mycore, scheduler* sch);

#endif