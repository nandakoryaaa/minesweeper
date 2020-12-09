#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "minesweeper.h"

int rnd(int max) {
    return rand() % max;
}

#include "icons.c"
#include "field_model.c"
#include "field_view.c"
#include "menu.c"

FieldModel fieldModel;
FieldView fieldView;

int main(int argc, char* argv[]) {
    srand(time(0));
    init_field(&fieldModel, 30, 16, (int) (30 * 16 / 6));
    fieldView.fieldModel = &fieldModel;
    fieldView.cell_size = 16;
    set_view_sizes(&fieldView);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Minesweeper",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        fieldView.width,
        fieldView.height,
        SDL_WINDOW_OPENGL
    );

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
    SDL_Surface *tmpSurface = SDL_CreateRGBSurfaceFrom(icons, 178, 32, 24, 536, 255 << 16, 255 << 8, 255, 0);
    SDL_Surface *imageSurface = SDL_ConvertSurface(tmpSurface, screenSurface->format, 0);

    fieldView.screenSurface = screenSurface;
    fieldView.imageSurface = imageSurface;
    fieldView.color_body = COLOR_BODY;
    fieldView.color_light = COLOR_LIGHT;
    fieldView.color_shadow = COLOR_SHADOW;
    fieldView.currentFace = &field_images[SMILEY_HAPPY];
    draw_field(&fieldView);
    SDL_UpdateWindowSurface(window);
 
    int game_mode = GAME_INIT;

    MouseBox startBox = {BOX_STARTBUTTON, 0, &fieldView.startButtonRect};
    MouseBox menuButtonBox = {BOX_MENUBUTTON, 0, &fieldView.menuButtonRect};
    MouseBox fieldBox = {BOX_FIELD, 0, &fieldView.fieldRect};
    MouseBox menuBox = {BOX_MENU, 0, &fieldView.fieldRect};
    MouseBox *box_list[] = {&startBox, &menuButtonBox, &fieldBox};
 
    SDL_Event event;
    MouseBox *activeBox = NULL;

    while (1) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }
        fieldView.need_redraw = 0;
        if (event.type == SDL_MOUSEMOTION && activeBox) {
            if (!in_rect(event.motion.x, event.motion.y, activeBox->rect)) {
                process_mouse_out(activeBox, &fieldView);
                activeBox = NULL;
            } else {
        		activeBox = process_mouse_move(activeBox, &event, &fieldView);
            }
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            for (int i = 0; i < 3; i++) {
                MouseBox *box = box_list[i];
                if (in_rect(event.button.x, event.button.y, box->rect)) {
                    activeBox = process_mouse_down(box, &event, &fieldView);
                    break;
                }
            }
        } else if (event.type == SDL_MOUSEBUTTONUP && activeBox) {
            process_mouse_up(activeBox, &event, &fieldView);
            activeBox = NULL;
        }
        if (fieldView.need_redraw) {
            SDL_UpdateWindowSurface(window);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
