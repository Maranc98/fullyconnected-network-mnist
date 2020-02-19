#include <allegro5/allegro.h>
#include <string>
#include <iostream>
#include "graphics.h"
#include "conf.h"

bool initialize_graphics(){
    if(!al_init()){
        printf("%s\n", ERROR_ALLEGRO_INIT);
        return false;
    }

    return true;
}

ALLEGRO_DISPLAY* create_display(int w, int h){

    return al_create_display(w, h);
}

bool destroy_display(ALLEGRO_DISPLAY* display){

    printf("Press enter to leave.");
    getchar();

    al_destroy_display(display);

    return true;
}
