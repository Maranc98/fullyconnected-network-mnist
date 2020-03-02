#include "read_mnist.h"
#include <fstream>
#include <arpa/inet.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "network.h"
#include "graphics.h"

using namespace std;

int read_mnist_labels(char path[], float ***outputs, unsigned int print_numbers /* = 0 */){
    ifstream fp(path, ios::in | ios::binary);
    uint32_t size;
    unsigned char byte;

    if(!fp.is_open()){
        printf("ERROR: Couldn't open mnist labels file.\n");
        return -1;
    }

    fp.read(reinterpret_cast<char*>(&size), sizeof(size));
    fp.read(reinterpret_cast<char*>(&size), sizeof(size));
    size = htonl(size);
    printf("Reading %d labels...\n", size);

    *outputs = new float*[size];
    float **outputs_ = *outputs;
    for(unsigned int i = 0; i < size; i++){
        outputs_[i] = new float[10];
        for(int j = 0; j < 10; j++){
            outputs_[i][j] = 0;
        }
    }

    if(print_numbers > 0)
        printf("{");
    for(unsigned int i = 0; i < size; i++){
        fp.read(reinterpret_cast<char*>(&byte), sizeof(byte));
        if(i < print_numbers)
            printf(" %d", byte);
        outputs_[i][byte] = 1;
    }
    if(print_numbers > 0)
        printf(" }\n");

    return size;
}

void print_mnist_labels(float **outputs, int lines, bool isfloat){
    for(int i = 0; i < lines; i++){
        printf("{\t");
        for(int j = 0; j < 10; j++){
            if(isfloat && outputs[i][j] > 0.005)
                printf(" %.2f\t", outputs[i][j]);
            else
                printf(" %.0f\t", outputs[i][j]);
        }
        printf(" }\n");
    }
}

int read_mnist_images(char path[], float ***inputs, int &x, int &y){
    ifstream fp(path, ios::in | ios::binary);
    uint32_t temp;
    int size;
    unsigned char byte;

    if(!fp.is_open()){
        printf("ERROR: Couldn't open mnist images file.\n");
        return -1;
    }

    fp.read(reinterpret_cast<char*>(&temp), sizeof(temp));
    fp.read(reinterpret_cast<char*>(&temp), sizeof(temp));
    size = htonl(temp);
    fp.read(reinterpret_cast<char*>(&temp), sizeof(temp));
    x = htonl(temp);
    fp.read(reinterpret_cast<char*>(&temp), sizeof(temp));
    y = htonl(temp);

    printf("Reading %d images(%dx%d)...\n", size, x, y);

    *inputs = new float*[size];
    float **inputs_ = *inputs;

    int input_size = x * y;
    for(int i = 0; i < size; i++){
        inputs_[i] = new float[input_size];
        for(int j = 0; j < input_size; j++){
            fp.read(reinterpret_cast<char*>(&byte), sizeof(byte));
            inputs_[i][j] = byte;
        }
    }

    return size;
}

void draw_mnist_image(float **inputs, int image_number, int img_x, int img_y, int x0, int y0, int w, int h){
    unsigned char transp;
    float w_step = (float)w / (float)img_x;
    float h_step = (float)h / (float)img_y;

    for(int i = 0; i < img_x; i++){
        for(int j = 0; j < img_y; j++){
            transp = 255 - inputs[image_number][i + j*img_y];
            al_draw_filled_rectangle(x0 + w_step*i, y0 + h_step*j, x0 + w_step*(i+1), y0 + h_step*(j+1), al_map_rgb(transp,transp,transp));
        }
    }
}

void draw_mnist_image_row(float **inputs, int first_image_number, int last_image_number, int img_x, int img_y, int x0, int y0, int w, int h){
    for(int i = first_image_number; i < last_image_number; i++){
        draw_mnist_image(inputs, i, img_x, img_y, x0 + (i-first_image_number)*w, y0, w, h);
    }
}

void run_mnist_training_demo(){
    int batch_size, x, y;
    float **moutputs, **minputs;
    float *errors;
    char train_labels_path[] = "data/train-labels-idx1-ubyte";
    char train_images_path[] = "data/train-images-idx3-ubyte";
    char save_path[] = "data/mnist_network.txt";

    ALLEGRO_DISPLAY *window = al_create_display(1300, 1050);

    // Get mnist dataset image and label data
    batch_size = read_mnist_labels(train_labels_path, &moutputs);
    print_mnist_labels(moutputs, 20);
    read_mnist_images(train_images_path, &minputs, x, y);
    draw_mnist_image_row(minputs, 0, 10, x, y, 0, 0, 100, 100);
    draw_mnist_image_row(minputs, 10, 20, x, y, 0, 100, 100, 100);
    al_flip_display();

    int input_size = x*y;
    Network net(input_size);
    net.add_fcl(input_size);
    net.add_fcl(32);
    net.add_fcl(10);

    net.train(minputs, moutputs, batch_size, &errors);

    net.export_network(save_path);

    for(int i = 0; i < 2; i++){
        plot_data(errors+i*30000, 30000, 0, (i+1)*500, 1300, 500, 2);
    }
    delete[] errors;

    al_flip_display();

    getchar();
    al_destroy_display(window);
}

void run_mnist_testing_demo(){
    int batch_size, x, y;
    float **moutputs, **minputs;
    float *errors;
    char test_labels_path[] = "data/t10k-labels-idx1-ubyte";
    char test_images_path[] = "data/t10k-images-idx3-ubyte";
    char save_path[] = "data/mnist_network.txt";

    ALLEGRO_DISPLAY *window = al_create_display(1300, 1050);

    Network net(0);
    net.import_network(save_path);

    // Testing network on dataset
    batch_size = read_mnist_labels(test_labels_path, &moutputs);
    read_mnist_images(test_images_path, &minputs, x, y);

    net.test(minputs, moutputs, batch_size, &errors);

    al_clear_to_color(al_map_rgb(0,0,0));
    plot_data(errors, 10000, 0, 500, 1300, 500, 2);
    delete[] errors;
    al_flip_display();

    getchar();
    al_destroy_display(window);
}

void run_mnist_step_by_step_test(){
    int batch_size, x, y;
    float **moutputs, **minputs;
    float *computed_outputs;
    char test_labels_path[] = "data/t10k-labels-idx1-ubyte";
    char test_images_path[] = "data/t10k-images-idx3-ubyte";
    char save_path[] = "data/mnist_network.txt";

    ALLEGRO_DISPLAY *window = al_create_display(1300, 1050);

    Network net(0);
    net.import_network(save_path);

    // Testing network on dataset
    batch_size = read_mnist_labels(test_labels_path, &moutputs);
    read_mnist_images(test_images_path, &minputs, x, y);

    for(int i = 0; i < batch_size; i++){
        computed_outputs = net.forward(minputs[i]);

        draw_mnist_image(minputs, i, x, y, 0, 0, 100, 100);
        printf("COMPUTED:\t");
        print_mnist_labels(&computed_outputs, 1, true);
        printf("ACTUAL:  \t");
        print_mnist_labels(moutputs+i);
        getchar();
    }

    al_destroy_display(window);
}
