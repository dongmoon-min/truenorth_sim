# truenorth_sim

introduction:
Timing simulator of TrueNorth micro-architecture of IBM for graduation project in Seoul National Univ.
TrueNorth is neuromorphic chip which is designed for simulating full human brain.
There is two type of simulator, first one is original TrueNorth timing simulator, and second one is upgraded timing simulator.
I found that routers and core to core spike sending mechanism are key bottle-neck of the system in the original TrueNorth, so I reinforced routers and protocols in upgraded TrueNorth simulator.

how to change simulator:
1. workload is generated in coresram.c, in "make_neuron_info" function. you can change options by simply changing defined parameters.
2. you can change simulating time by changing "SIMTIME" param which is defined in "main.c".
3. you can change delay of modules by simply changing defined parameters. Almost of parameters are defined in local modules (i,e, neuronblock.c, router.c, scheduler.c, tokencontroller.c, coresram.c)

usuage:
1. type "cd /truenorth" or "cd /TrueNorth upgrade" and type "make" to make executable files. 
2. "./truenorth -h" will show available options of simulators.
3. basically, "./truenorth" will initiate simulation, but if you want to see some graphic of activation rate of router, type "./truenorth -g". 

* if you have more question, please connect me, ehdans06@snu.ac.kr
