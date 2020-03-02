#ifndef LAYERS_H
#define LAYERS_H

#include <fstream>

enum LayerType {fcl, softmax};

class Layer {
    // Are exported
    float** weight_;
    float* bias_;

    int size_, input_size_;

    // Not exported
    float* y_;
    float* delta_;
    float* x_;

    float eta_;

    float init_weight();
public:
    Layer(int input_size, int size);
    void forward(float x[], float **y);
    void backward(float t[], float **d);
    void export_layer(char path[]);
    void import_layer(FILE* fp);
};

#endif
