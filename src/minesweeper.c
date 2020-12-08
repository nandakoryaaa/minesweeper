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
    draw_field(&fieldView);
    SDL_UpdateWindowSurface(window);

    unsigned char *current_cell = NULL;
    int cell_x;
    int cell_y;
    int cell_size = fieldView.cell_size;

    SDL_Rect fieldRect = fieldView.fieldRect;
    SDL_Event event;

    while (1) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }
        fieldView.need_redraw = 0;
        if (event.type == SDL_MOUSEMOTION && current_cell) {
            if (!in_rect(event.motion.x, event.motion.y, &fieldRect)
                || (event.motion.x - fieldRect.x) / cell_size != cell_x
                || (event.motion.y - fieldRect.y) / cell_size != cell_y
            ) {
                draw_closed_cell(&fieldView, cell_x, cell_y);
                current_cell = NULL;
            }
        } else if (event.type == SDL_MOUSEBUTTONDOWN && in_rect(event.button.x, event.button.y, &fieldRect)) {
            cell_x = (event.button.x - fieldRect.x) / cell_size;
            cell_y = (event.button.y - fieldRect.y) / cell_size;
            current_cell = fieldModel.field + cell_y * fieldModel.width + cell_x;
            if (*current_cell & F_CLOSED) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if  (!(*current_cell & (F_FLAG | F_QUESTION))) {
                        draw_open_cell(&fieldView, cell_x, cell_y);
                    } else {
                        current_cell = NULL;
                    }
                } else {
                    switch_flag(current_cell);
                    draw_field(&fieldView);
                    current_cell = NULL;
                }
            } else {
                current_cell = NULL;
            }
        } else if (event.type == SDL_MOUSEBUTTONUP && current_cell) {
            open_cell(&fieldModel, cell_x, cell_y);
            draw_field(&fieldView);
            current_cell = NULL;
        }

        if (fieldView.need_redraw) {
            SDL_UpdateWindowSurface(window);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
