#include "read_mnist.h"
#include <fstream>
#include <arpa/inet.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "network.h"
#include "graphics.h"

using namespace std;

char train_labels_path[] = "data/train-labels-idx1-ubyte";
char train_images_path[] = "data/train-images-idx3-ubyte";
char test_labels_path[] = "data/t10k-labels-idx1-ubyte";
char test_images_path[] = "data/t10k-images-idx3-ubyte";
char save_path[] = "data/mnist_network.txt";

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

int get_mnist_array_to_int(float *outputs){
    float max = 0, index = 0;
    for(int i = 0; i < 10; i++){
        if(outputs[i] > max){
            max = outputs[i];
            index = i;
        }
    }
    return index;
}

void print_confusion_matrix(int confusion_matrix[10][10]){
    printf("\nCONFUSION MATRIX:\n");
    printf("Row number is the correct number, column number is the computed guess.\n\t");
    for(int i = 0; i < 10; i++)
        printf("%d\t", i);
    printf("\n\t");
    for(int i = 0; i < 10; i++)
        printf("-------\t");
    printf("\n  |\n");
    for(int i = 0; i < 10; i++){
        printf("%d |\t", i);
        for(int j = 0; j < 10; j++){
            printf("%d\t", confusion_matrix[i][j]);
        }
        printf("\n  |\n");
    }
}

void draw_confusion_matrix(int confusion_matrix[10][10]){
    float width = 880, height = 955;
    float step = 80;
    ALLEGRO_DISPLAY *window = al_create_display(width, height);
    ALLEGRO_FONT *font = al_load_ttf_font("data/RobotoMono-Regular.ttf", 20, 0);
    int max = 0;
    int color_shade, opposite_color, color_tolerance = 60;
    char text[110];

    // Get max value to color the boxes accordingly
    for(int i = 0; i < 10; i++)
        for(int j = 0; j < 10; j++)
            if(max < confusion_matrix[i][j])
                max = confusion_matrix[i][j];

    // Draws the matrix content
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            color_shade = 255 * ( 1 - (float)confusion_matrix[i][j]/(float)max);
            al_draw_filled_rectangle(step*(i+1), step*(j+1), step*(i+2), step*(j+2), al_map_rgb(color_shade,color_shade,color_shade));
            sprintf(text, "%d", confusion_matrix[i][j]);
            opposite_color = 255 - color_shade;
            if(opposite_color - color_shade < color_tolerance && opposite_color - color_shade > -color_tolerance)
                opposite_color = 255;
            al_draw_text(font, al_map_rgb(opposite_color,opposite_color,opposite_color), step*(i+1.5), step*(j+1.5) - 10, ALLEGRO_ALIGN_CENTRE, text);
        }
    }

    // Draws matrix descriptions
    al_draw_rectangle(0, 0, step, step, al_map_rgb(255,0,255),2);
    for(int i = 0; i < 10; i++){
        sprintf(text, "%d", i);
        al_draw_text(font, al_map_rgb(255,255,255), step*(i+1.5), step*(0.5) - 10, ALLEGRO_ALIGN_CENTRE, text);
        al_draw_text(font, al_map_rgb(255,255,255), step*(0.5), step*(i+1.5) - 10, ALLEGRO_ALIGN_CENTRE, text);
        al_draw_rectangle(step*(i+1), 0, step*(i+2), step, al_map_rgb(255,0,0),2);
        al_draw_rectangle(0, step*(i+1), step, step*(i+2), al_map_rgb(0,255,0),2);
    }
    sprintf(text, "Green boxes contain the correct result.");
    al_draw_text(font, al_map_rgb(255,255,255), 880/2, 890, ALLEGRO_ALIGN_CENTRE, text);

    sprintf(text, "Red boxes contain the guessed result computed by the network.");
    al_draw_text(font, al_map_rgb(255,255,255), 880/2, 920, ALLEGRO_ALIGN_CENTRE, text);

    al_flip_display();
    getchar();

    al_destroy_display(window);
}

void run_mnist_training_demo(){
    int batch_size, x, y;
    int window_width = 1000, window_height = 250;
    float **moutputs, **minputs;
    float *errors;
    char text[100];
    int total_examples = 20;

    ALLEGRO_DISPLAY *window = al_create_display(window_width, window_height);
    ALLEGRO_FONT *font = al_load_ttf_font("data/RobotoMono-Regular.ttf", 20, 0);

    // Get mnist dataset image and label data
    batch_size = read_mnist_labels(train_labels_path, &moutputs);
    printf("\nHere are shown %d output examples:\n", total_examples);
    print_mnist_labels(moutputs, total_examples);
    read_mnist_images(train_images_path, &minputs, x, y);

    sprintf(text, "Above are shown 20 examples of mnist dataset inputs.");
    al_draw_text(font, al_map_rgb(255,255,255), window_width/2, window_height-40, ALLEGRO_ALIGN_CENTRE, text);
    draw_mnist_image_row(minputs, 0, 10, x, y, 0, 0, 100, 100);
    draw_mnist_image_row(minputs, 10, 20, x, y, 0, 100, 100, 100);
    al_flip_display();

    int input_size = x*y;
    Network net(input_size);
    net.add_fcl(input_size);
    net.add_fcl(32);
    net.add_fcl(10);

    net.train(minputs, moutputs, batch_size, &errors);

    al_destroy_display(window);
    window_width = 1000;
    window_height = 1000;
    window = al_create_display(window_width, window_height);

    net.export_network(save_path);

    for(int i = 0; i < 2; i++){
        sprintf(text, "Plotting error from entry %d to %d (online training).\n", i*batch_size/2, (i+1)*batch_size/2);
        al_draw_text(font, al_map_rgb(255,255,255), window_width/2, i*window_height/2 + 30, ALLEGRO_ALIGN_CENTRE, text);
        plot_data(errors+i*batch_size/2, batch_size/2, 0, (i+1)*window_height/2, window_width, window_height/2 - 100, 1);
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
    char text[100];
    int window_width = 1000, window_height = 550;

    Network net(0);
    net.import_network(save_path);

    // Testing network on dataset
    batch_size = read_mnist_labels(test_labels_path, &moutputs);
    read_mnist_images(test_images_path, &minputs, x, y);

    net.test(minputs, moutputs, batch_size, &errors);

    ALLEGRO_DISPLAY *window = al_create_display(window_width, window_height);
    ALLEGRO_FONT *font = al_load_ttf_font("data/RobotoMono-Regular.ttf", 20, 0);

    al_clear_to_color(al_map_rgb(0,0,0));
    sprintf(text, "Plotting error in test dataset (%d entries).\n", batch_size);
    al_draw_text(font, al_map_rgb(255,255,255), window_width/2, window_height-40, ALLEGRO_ALIGN_CENTRE, text);
    plot_data(errors, batch_size, 0, 500, 1000, 500, 1);
    delete[] errors;
    al_flip_display();

    getchar();
    al_destroy_display(window);
}

void run_mnist_testing_demo_confusion_matrix(){
    int batch_size, x, y;
    float **moutputs, **minputs;
    float *computed_outputs;
    int computed_number, correct_number;
    int confusion_matrix[10][10];

    Network net(0);
    net.import_network(save_path);

    // Testing network on dataset
    batch_size = read_mnist_labels(test_labels_path, &moutputs);
    read_mnist_images(test_images_path, &minputs, x, y);

    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            confusion_matrix[i][j] = 0;
        }
    }

    for(int i = 0; i < batch_size; i++){
        computed_outputs = net.forward(minputs[i]);
        computed_number = get_mnist_array_to_int(computed_outputs);
        correct_number = get_mnist_array_to_int(moutputs[i]);

        confusion_matrix[computed_number][correct_number]++;
    }

    print_confusion_matrix(confusion_matrix);
    draw_confusion_matrix(confusion_matrix);
}

void run_mnist_testing_demo_step_by_step(){
    int batch_size, x, y;
    float **moutputs, **minputs;
    float *computed_outputs;

    ALLEGRO_DISPLAY *window = al_create_display(100, 100);

    Network net(0);
    net.import_network(save_path);

    // Testing network on dataset
    batch_size = read_mnist_labels(test_labels_path, &moutputs);
    read_mnist_images(test_images_path, &minputs, x, y);

    for(int i = 0; i < batch_size; i++){
        computed_outputs = net.forward(minputs[i]);

        draw_mnist_image(minputs, i, x, y, 0, 0, 100, 100);
        al_flip_display();
        printf("COMPUTED:\t");
        print_mnist_labels(&computed_outputs, 1, true);
        printf("ACTUAL:  \t");
        print_mnist_labels(moutputs+i);
        getchar();          // TODO mettere invece che legge pulsante da allegro
    }

    al_destroy_display(window);
}
