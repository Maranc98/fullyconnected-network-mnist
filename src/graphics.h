#ifndef DISPLAY_H
#define DISPLAY_H
#include <allegro5/allegro.h>

bool initialize_graphics();
ALLEGRO_DISPLAY* create_display(int, int);
bool destroy_display(ALLEGRO_DISPLAY*);

#endif
