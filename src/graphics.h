#ifndef DISPLAY_H
#define DISPLAY_H
#include <allegro5/allegro.h>

bool initialize_graphics();
ALLEGRO_DISPLAY* create_display(int, int);
bool destroy_display(ALLEGRO_DISPLAY*);
void plot_data(float errors[], int batch_size, int x0, int y0, int w, int h, int mode = 0);

#endif
