#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <string>
#include <iostream>
#include "graphics.h"
#include "conf.h"

bool initialize_graphics(){
    if(!al_init()){
        printf("%s\n", ERROR_ALLEGRO_INIT);
        return false;
    }

    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

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

void plot_data(float errors[], int batch_size, int x0, int y0, int w, int h, int mode){
    int width = 1;
    float step = (float)(w)/(float)batch_size;
    float error;
    int mini_batch_size = 100;

    ALLEGRO_COLOR x_color = al_map_rgb(0,255,0);

    switch(mode){
        case 0:
            for(int i = 0; i < batch_size - 1; i++){
                al_draw_line(x0 + step*i, y0 - h*errors[i], x0 + step*(i+1), y0 - h*errors[i + 1], al_map_rgb(100,100,100), 1);
            }
            al_draw_line(x0, y0, x0 + w, y0, x_color, width);
            break;
        case 1:
            for(int i = 0; i < batch_size; i++){
                al_draw_pixel(x0 + step*i, y0 - h*errors[i], al_map_rgb(255,255,255));
            }
            al_draw_line(x0, y0, x0 + w, y0, x_color, width);
            break;
        case 2:
            // TODO: APPARENTLY THIS DOESNT WORK
            step = (float)(w)/(float)(batch_size/mini_batch_size);
            for(int i = 0; i < batch_size/mini_batch_size; i++){
                error = 0;
                for(int j = 0; j < mini_batch_size; j++){
                    error += errors[i*mini_batch_size + j]/mini_batch_size;
                }
                al_draw_pixel(x0 + step*i, y0 - h*error, al_map_rgb(255,255,255));
            }
            al_draw_line(x0, y0, x0 + w, y0, x_color, width);
            break;
        default:
            break;
    }
}
