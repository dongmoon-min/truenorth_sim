#include "core.h"

#define NINFOSEND_DELAY 4
#define SRMQUEUE_SIZE   1

void get_neuron_info (sram* srm, char* ch);

void sram_init (sram* srm, char* ch) {

    memset ((void*)&(srm->timer), 0, sizeof(sram_t));
    memset ((void*)&(srm->ninfo), 0, sizeof(neuron_info) * NEURONS);
    queue_init (&(srm->rq), SRMQUEUE_SIZE);

    get_neuron_info (srm, ch);
    
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

// file descriptor for function, "get_neuron_info"
FILE* fd = NULL;

void get_neuron_info (sram* srm, char* ch) {
    
    int i, j, data;
    
    // open file which contains neuron info
    if (fd == NULL) {
        fd = fopen (ch, "r");
        if (fd == NULL) {
            printf ("File open failed.\n");
            printf ("help: ./truenorth -h\n");
            exit (-1);
        }
    }
    // get neuron data from the file
    for (i = 0; i < NEURONS; i++) {
        // get synapse info
        for (j = 0; j < AXON_NUMBER; j++) {
            fscanf (fd, "%d ", &data);
            srm->ninfo[i].synapse[j] = data;
        }
        // get weight info
        for (j = 0; j < AXON_NUMBER; j++) {
            fscanf (fd, "%d ", &data);
            srm->ninfo[i].weight[j] = data;
        }
        // get leak info
        fscanf (fd, "%d ", &data);
        srm->ninfo[i].leak = data;
        // get destination info (i,e, dest, des_axon, tick)
        fscanf (fd, "%d ", &data);
        srm->ninfo[i].dest = data;
        fscanf (fd, "%d ", &data);
        srm->ninfo[i].des_axon = data;
        fscanf (fd, "%d ", &data);
        srm->ninfo[i].tick = data;
        // get nopt info (type of neuron)
        fscanf (fd, "%d\n", &data);
        srm->ninfo[i].nopt = data;
    }

    return;
}