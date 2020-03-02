#ifndef READ_MNIST_H
#define READ_MNIST_H

int read_mnist_labels(char path[], float ***outputs, unsigned int print_numbers = 0);
void print_mnist_labels(float **outputs, int lines = 1, bool isfloat = false);

int read_mnist_images(char path[], float ***inputs, int &x, int &y);
void draw_mnist_image(float **inputs, int image_number, int img_x, int img_y, int x0, int y0, int w, int h);
void draw_mnist_image_row(float **inputs, int first_image_number, int last_image_number, int img_x, int img_y, int x0, int y0, int w, int h);

void run_mnist_training_demo();
void run_mnist_testing_demo();
void run_mnist_step_by_step_test();
#endif
