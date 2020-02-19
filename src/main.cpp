#include <string>
#include <iostream>
#include <allegro5/allegro.h>
#include "graphics.h"
#include "conf.h"
#include "neuron.h"

using namespace std;

double identita(double num){
    return num;
}

int main(int argc, char **argv){

    if(!initialize_graphics())
        return 1;

    //ALLEGRO_DISPLAY *MAIN_SCREEN = create_display(1024, 920);
    InputNeuron<double> neurone0(0,1,0.5), neurone1(0,1,0.127);
    Neuron<double>  neurone2(2,0,0.333);
    printf("Numero neuroni connessi a neuron2: %d\n", neurone2.connect_input_neuron(&neurone0));
    printf("Numero neuroni connessi a neuron2: %d\n", neurone2.connect_input_neuron(&neurone1));
    neurone2.set_activation_function(&identita);
    neurone2.initialize_weights();
    neurone0.set_input_value(100);
    neurone1.set_input_value(1000);

    for(int i = 0; i < 1; i++){
        printf("%d) Executing stuff...\n", i);
        neurone2.compute_output_value();
        sleep(0);
    }
    printf("\nDone!\n");

    printf("Valore del neurone 2: %f\n", neurone2.get_output_value());

    //destroy_display(MAIN_SCREEN);

    return 0;
}
