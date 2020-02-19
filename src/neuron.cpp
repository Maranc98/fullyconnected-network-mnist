#include "neuron.h"
#include <iostream>

template <class numeric>
Neuron<numeric>::Neuron(int in, int out, numeric b){
    input_size = 0;
    output_size = 0;
    bias = b;
    input_weights = new numeric[in];
    input_neurons = new Neuron*[in];
    output_neurons = new Neuron*[out];
}

/* Initializes weights as zero */
/* Note that it would be preferable to assign them randomly with gaussian distribution */
template <class numeric>
void Neuron<numeric>::initialize_weights(){
    for(int i = 0; i < input_size; i++){
        input_weights[i] = 0;
    }
}

/* Adds a new input neuron */
/* Returns the number of total neurons */
template <class numeric>
int Neuron<numeric>::connect_input_neuron(Neuron* neurone_input){
    input_neurons[input_size] = neurone_input;
    input_size++;

    return input_size;
}

/* Returns input size */
template <class numeric>
int Neuron<numeric>::get_input_size(){
    return input_size;
}

/* Returns the output value */
template <class numeric>
numeric Neuron<numeric>::get_output_value(){
    return output_value;
}

/* Sets the activation_function for the neuron */
template <class numeric>
void Neuron<numeric>::set_activation_function(numeric (*f)(numeric)){
    activation_function = f;
}

/* Updates the output value of the neuron depending on the input neurons' */
template <class numeric>
void Neuron<numeric>::compute_output_value(){
    numeric sum = bias;
    for(int i = 0; i < input_size; i++){
        sum += input_neurons[i]->output_value * input_weights[i];
    }

    output_value = (*activation_function)(sum);
}

template <class numeric>
InputNeuron<numeric>::InputNeuron(int in, int out, numeric b):Neuron<numeric>(in,out,b){
    ;
}

template <class numeric>
void InputNeuron<numeric>::set_input_value(numeric v){
    this->output_value = v;
}

template class Neuron<float>;
template class InputNeuron<float>;
template class Neuron<double>;
template class InputNeuron<double>;
