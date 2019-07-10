#include "Simulator.h"


using namespace std;
using namespace szx;


int main() {
    //Simulator::initDefaultEnvironment();

    Simulator sim;
    //sim.debug();
    sim.benchmark(1);
    //sim.parallelBenchmark(1);
    //sim.generateInstance();
    //sim.convertIgdpInstances();
    //sim.convertDbdpInstances();
    //sim.convertDbdp30Instances();
    //sim.convertDbdp1000Instances();

    return 0;
}
