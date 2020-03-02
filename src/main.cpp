#include <string>
#include <iostream>
#include <fstream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
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

    int choice;
    printf("Choose one action:\n");
    printf("1) Train a fully connected network using the mnist dataset.\n");
    printf("2) Test the saved network and with the mnist testing dataset.\n");
    printf("3) Test the saved network and print a confusion matrix to evaluate performance.\n");
    printf("4) Test the saved network step by step and compare the results.\n");
    scanf("%d", &choice);
    getchar();

//  MNIST TEST
    switch(choice){
        case 1:
            run_mnist_training_demo();
            break;
        case 2:
            run_mnist_testing_demo();
            break;
        case 3:
            run_mnist_testing_demo_confusion_matrix();
            break;
        case 4:
            run_mnist_testing_demo_step_by_step();
            break;
        default:
            break;
    }

    return 0;
}
