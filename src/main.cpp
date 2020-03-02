#include <string>
#include <iostream>
#include <fstream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cstdlib>
#include <ctime>
#include <arpa/inet.h>
#include "graphics.h"
#include "conf.h"
#include "neuron.h"
#include "layers.h"
#include "network.h"
#include "read_mnist.h"

using namespace std;

int main(int argc, char **argv){

    if(!initialize_graphics())
        return 1;

    srand(time(0));

//  MNIST TEST
    //run_mnist_training_demo();
    //run_mnist_testing_demo();
    run_mnist_step_by_step_test();
    
    return 0;
}
