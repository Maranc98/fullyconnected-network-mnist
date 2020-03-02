#include "network.h"
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

Network::Network(int input_size): input_size_(input_size){
    layers_ = 0;
};

// Returns last layer descriptor pointing to an actual layer
// Returns 0 if head descriptor is not initialized yet
layer_descriptor* last_layer(layer_descriptor* head){
    layer_descriptor *prec;
    int depth;

    if(!head){
        //printf("LAST_LAYER(): Last layer is the head layer.\n");
        return 0;
    }

    depth = 0;
    while(head){
        depth++;
        prec = head;
        head = head->next_layer;
    }
    //printf("LAST_LAYER(): Last layer is layer number %d.\n", depth);
    return prec;
}

void Network::add_fcl(int size){
    layer_descriptor *ld, *last;
    int last_layer_size;

    ld = new layer_descriptor;
    last = last_layer(layers_);

    if(!last){
        printf("NETWORK::ADD_FCL(%d): Adding fully connected layer as head layer.\n", size);
        layers_ = ld;
        ld->prec_layer = 0;
        last_layer_size = input_size_; //TODO inizializzare il network con input size generico
    } else {
        printf("NETWORK::ADD_FCL(%d): Adding fully connected layer. (Not head layer)\n", size);
        last->next_layer = ld;
        ld->prec_layer = last;
        last_layer_size = last->size;
    }

    ld->layer_type = fcl;
    ld->layer = new Layer(last_layer_size, size);
    ld->size = size;
    ld->next_layer = 0;
    output_size_ = size;
}

float* Network::forward(float input[]){
    float *current_input, *current_output;
    current_input = input;

    for(layer_descriptor* current_layer = layers_; current_layer; current_layer = current_layer->next_layer){
        current_layer->layer->forward(current_input, &current_output);

        /*
        printf("OUTPUTS: ");
        for(int i = 0; i < current_layer->size; i++){
            printf("| %f ", current_output[i]);
        }
        printf("|\n\n");
        */

        current_input = current_output;
    }
    /*
    printf("OUTPUT: ");
    for(int i = 0; i < output_size_; i++){
        printf("| %f ", current_output[i]);
    }
    printf("|\n");
    */
    y_ = current_output;
    return y_;
}

float Network::backward(float t[]){
    // calcola differenza = t - y
    float *diff = new float[output_size_];
    float error;
    bool relevant = true;

                                        //printf("\t--- TRAINING\n");

    error = 0;
                                        //printf("DELTA IN OUTPUT: ");
    for(int i = 0; i < output_size_; i++){
        diff[i] = t[i] - y_[i];
        error += (diff[i] * diff[i]);
        if(diff[i] > 0.1 || diff[i] < -0.1)
            relevant = false;
                                            //printf("%.3f ", diff[i]);
    }
                                        //printf("\nERRORE: %f\n\n", error/2);

    float *current_delta, *next_delta;
    current_delta = diff;
    // Per ogni layer dall'ultimo al secondo [L -> 2]
    for(layer_descriptor* current_layer = last_layer(layers_); current_layer->prec_layer; current_layer = current_layer->prec_layer){
        current_layer->layer->backward(current_delta, &next_delta);

        /*
        if(relevant){
            printf("PASSED DELTAS: ");
            for(int i = 0; i < current_layer->prec_layer->size; i++){
                printf("| %f ", next_delta[i]);
            }
            printf("|\n\n");
        }
        */

        current_delta = next_delta;
    }

    delete[] diff;
    return error/2;
}

void Network::train(float **inputs, float **outputs, int batch_size, float **save_error /* = 0 */){
    float *error = new float[batch_size];
    float average_error = 0;
    float percentage_threshold = 0;
    int percentage = 0;

    for(int i = 0; i < batch_size; i++){
        if(i > percentage_threshold){
            printf("%d%% done...\n", percentage);
            percentage_threshold += batch_size/100;
            percentage++;
        }

        forward(inputs[i]);
        error[i] = backward(outputs[i]);
        average_error += error[i] / batch_size;
    }

    printf("AVERAGE TRAINING ERROR: %f\n", average_error);

    if(save_error != 0)
        *save_error = error;
    else
        delete[] error;
}

void Network::test(float **inputs, float **outputs, int batch_size, float **save_error /* = 0 */){
    float *error = new float[batch_size];
    float *output;
    float average_error = 0;
    float percentage_threshold = 0;
    int percentage = 0;

    for(int i = 0; i < batch_size; i++){
        if(i > percentage_threshold){
            printf("%d%% done...\n", percentage);
            percentage_threshold += batch_size/100;
            percentage++;
        }

        output = forward(inputs[i]);

        /*
        printf("EXPECTED OUTPUT vs COMPUTED OUTPUT: (");
        for(int j = 0; j < output_size_; j++){
            printf(" %.2f", outputs[i][j]);
        }
        printf(" ) (");
        for(int j = 0; j < output_size_; j++){
            printf(" %.2f", output[j]);
        }
        printf(" )\n");
        */

        error[i] = 0;
        for(int j = 0; j < output_size_; j++){
            float temp = outputs[i][j] - output[j];
            error[i] += temp*temp;
        }
        error[i] /= 2;
        //printf("ERRORE: %f\n", error[i]);
        average_error += error[i]/batch_size;
    }

    printf("AVERAGE TESTING ERROR: %f\n", average_error);
    if(save_error != 0)
        *save_error = error;
    else
        delete[] error;
}

void Network::export_network(char path[]){
    FILE* fp = fopen(path, "w");

    printf("\nNETWORK::EXPORT_NETWORK(%s)\n", path);

    if(!fp){
        printf("ERROR: Couldn't open export file.\n");
        return;
    }

    fprintf(fp, "");
    fclose(fp);

    for(layer_descriptor* current = layers_; current; current = current->next_layer){
        fp = fopen(path, "a+");
        fprintf(fp, "%d\n%d\n", current->layer_type, current->size);
        fclose(fp);
        current->layer->export_layer(path);
    }
}

void Network::import_network(char path[]){
    FILE* fp = fopen(path, "r");
    int layer_type;
    int size;
    layer_descriptor* current_layer;
    bool first_iteration = true;

    printf("\nNETWORK::IMPORT_NETWORK(%s)\n", path);

    if(!fp){
        printf("ERROR: Couldn't open export file.\n");
        return;
    }

    layers_ = 0;            // TODO mettere un delete layers che li cancella iterativamente

    while(!feof(fp)){
        fscanf(fp, "%d\n", &layer_type);
        fscanf(fp, "%d\n", &size);

        if(first_iteration){
            first_iteration = false;
            input_size_ = size;
        }

        switch(layer_type){
            case fcl:
                this->add_fcl(size);
                break;
            default:
                printf("Tipo di layer non riconosciuto.\n");
                break;
        }

        current_layer = last_layer(layers_);
        current_layer->layer->import_layer(fp);
    }

    fclose(fp);
}
