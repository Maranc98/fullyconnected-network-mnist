#ifndef NEURON_H
#define NEURON_H

template <class numeric>
class Neuron {
    private:
        Neuron** input_neurons;
        Neuron** output_neurons;
        numeric* input_weights;
        numeric bias;
        int input_size;
        int output_size;
        numeric (*activation_function)(numeric);
    protected:
        numeric output_value;
    public:
        Neuron(int in, int out, numeric b);
        //Neuron(int in, int out, int b, numeric (*f)(numeric));

        // Set the weights to their default value
        void initialize_weights();

        // Connects a new input neuron
        int connect_input_neuron(Neuron* n);

        // Returns input size
        int get_input_size();

        // Get output_value (after activation)
        numeric get_output_value();

        // Sets the activation function
        void set_activation_function(numeric (*f)(numeric));

        // Updates the output_value through the activation function
        void compute_output_value();
};

template <class numeric>
class InputNeuron: public Neuron<numeric> {
    public:
        InputNeuron(int in, int out, numeric b);
        void set_input_value(numeric v);
};

#endif
