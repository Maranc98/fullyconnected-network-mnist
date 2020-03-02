#include "layers.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>

float Layer::init_weight(){
    return (4 * ((float)rand() / (float)RAND_MAX) - 2)/input_size_; // Random da -2 a 2
}

float init_bias(){
    return 2 * ((float)rand() / (float)RAND_MAX) - 1; // Random da -1 a 1
}

float sigmoid(float input){
    return 1 / (1 + exp(-input));
}

float activation_function(float input){
    return sigmoid(input);
}

Layer::Layer(int input_size, int size): size_(size), input_size_(input_size){
    weight_ = new float*[size];
    bias_ = new float[size];
    y_ = new float[size];
    delta_ = new float[input_size];
    x_ = new float[input_size];
    eta_ = 0.2;

    for(int i = 0; i < size; i++){
        weight_[i] = new float[input_size];
        bias_[i] = init_bias();
        //printf("\nBias [%d] inizializzato come %f.\n", i, bias_[i]);
        for(int j = 0; j < input_size; j++){
            weight_[i][j] = init_weight();
            //printf("Peso [%d,%d] inizializzato come %f.\n", i, j, weight_[i][j]);
        }
    }
}

void Layer::forward(float x[], float** y){
    float somma;

    for(int j = 0; j < input_size_; j++){
        x_[j] = x[j];
    }

    for(int i = 0; i < size_; i++){
        somma = bias_[i];
        //printf("\n%d) bias(%.2f) ", i, bias_[i]);
        for(int j = 0; j < input_size_; j++){
            somma += weight_[i][j] * x[j];
            //printf("+ weight_[%d](%.2f) * x_[%d](%.2f) ", j, weight_[i][j], j, x[j]);
        }
        //printf("= somma(%.2f)", somma);
        y_[i] = activation_function(somma);
        //printf("\nOutput = %.2f\n", y_[i]);
    }
    *y = y_;
}

void Layer::backward(float current_delta[], float** next_delta){
    // Per ogni neurone, per ogni peso
        // Valore in input del peso * delta ricevuto per quel neurone *
        //      * derivata_funzione_attivazione(a in output di quel neurone) *
        //      * eta

    for(int j = 0; j < input_size_; j++){
        delta_[j] = 0;
    }

    for(int i = 0; i < size_; i++){
        float derivata_sigmoide = y_[i] * (1 - y_[i]);
        for(int j = 0; j < input_size_; j++){
            delta_[j] += weight_[i][j] * current_delta[i];
            weight_[i][j] += current_delta[i] * x_[j] * derivata_sigmoide * eta_;
        }
    }
    *next_delta = delta_;
    //eta_ = eta_ * 0.7;
}

void Layer::export_layer(char path[]){
    FILE* fp = fopen(path, "a+");

    if(!fp){
        printf("ERROR: Couldn't open export file.\n");
        return;
    }

    fprintf(fp, "%d %d\n", size_, input_size_);
    for(int i = 0; i < size_; i++){
        fprintf(fp, "%f ", bias_[i]);
        for(int j = 0; j < input_size_; j++){
            fprintf(fp, "%f ", weight_[i][j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

void Layer::import_layer(FILE *fp){
    fscanf(fp, "%d %d\n", &size_, &input_size_);
    printf("Importing fcl(%d, %d)\n", input_size_, size_);
    for(int i = 0; i < size_; i++){
        fscanf(fp, "%f ", &bias_[i]);
        for(int j = 0; j < input_size_; j++){
            fscanf(fp, "%f ", &weight_[i][j]);
        }
        fscanf(fp, "\n");
    }
}
