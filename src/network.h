#ifndef NETWORK_H
#define NETWORK_H

#include "layers.h"

struct layer_descriptor{
    layer_descriptor *next_layer;
    layer_descriptor *prec_layer;
    Layer* layer;
    int size;
    LayerType layer_type;
};

class Network{
    private:
        layer_descriptor* layers_;
        int input_size_, output_size_;

        float *y_;
    public:
        Network(int input_size);
        void add_fcl(int size);
        float* forward(float input[]);
        float backward(float t[]);
        void train(float **inputs, float **outputs, int batch_size, float **save_error = 0);
        void test(float **inputs, float **outputs, int batch_size, float **save_error = 0);
        void export_network(char path[]);
        void import_network(char path[]);
};

#endif
