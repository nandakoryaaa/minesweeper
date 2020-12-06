#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "minesweeper.h"

#include "icons.c"
#include "draw_image.c"
#include "draw_field.c"
#include "draw_panel.c"

unsigned char field[MAX_FIELD_SIZE];

int rnd(int max) {
    return rand() % max;
}

int init_field(
    unsigned char field[], int width, int height, int mine_count
) {
    int i;
    int total = width * height;

    if (total > MAX_FIELD_SIZE || mine_count > total) {
        return 0;
    }

    for (i = 0; i < total; i++) {
        field[i] = F_CLOSED;
    }

    int mine_x, mine_y, mine_addr;

    for (i = 0; i < mine_count; i++) {
        do {
            mine_x = rnd(width);
            mine_y = rnd(height);
            mine_addr = mine_y * width + mine_x;
        } while (field[mine_addr] & F_MINE);

        field[mine_addr] |= F_MINE;

        if (mine_y > 0) {
            field[mine_addr - width]++;
            if (mine_x > 0) {
                field[mine_addr - width - 1]++;
            }
            if (mine_x < width - 1) {
                field[mine_addr - width + 1]++;
            }
        }
        if (mine_x > 0) {
            field[mine_addr - 1]++;
        }
        if (mine_x < width - 1) {
            field[mine_addr + 1]++;
        }
        if (mine_y < height - 1) {
            field[mine_addr + width]++;
            if (mine_x > 0) {
                field[mine_addr + width - 1]++;
            }
            if (mine_x < width - 1) {
                field[mine_addr + width + 1]++;
            }
        }
    }

    return 1;
}

void set_window_rect(int w, int h, int cell_size, SDL_Rect *rect) {
    rect->w = w * cell_size + (3 + 6 + 3) * 2;
    rect->h = h * cell_size + 3 + 6 + 3 + 3 + 49 + 3;
}

void set_field_rect(int w, int h, int cell_size, SDL_Rect *rect) {
    rect->w = w * cell_size;
    rect->h = h * cell_size;
    rect->x = 3 + 6 + 3;
    rect->y = 3 + 49 + 3;
}

int in_rect(int x, int y, SDL_Rect *rect) {
    return x >= rect->x && y >= rect->y && x < rect->x + rect->w & y < rect->y + rect->h;
}

int main(int argc, char* argv[]) {

    SDL_Window *window;
    SDL_Event event;
    SDL_Rect windowRect;
    SDL_Rect fieldRect;
    int field_w = 30;
    int field_h = 16;
    int cell_size = 16;
    int game_mode = MODE_PLAY;

    srand(time(0));
    init_field(field, field_w, field_h, (int) field_w * field_h / 6);
    set_window_rect(field_w, field_h, cell_size, &windowRect);
    set_field_rect(field_w, field_h, cell_size, &fieldRect);

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "Minesweeper",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowRect.w,
        windowRect.h,
        SDL_WINDOW_OPENGL
    );

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
    SDL_Surface *tmpSurface = SDL_CreateRGBSurfaceFrom(icons, 178, 32, 24, 536, 255 << 16, 255 << 8, 255, 0);
    SDL_Surface *imgSurface = SDL_ConvertSurface(tmpSurface, screenSurface->format, 0);

    draw_field(screenSurface, imgSurface, field, field_w, field_h, cell_size);
    
    SDL_UpdateWindowSurface(window);

    unsigned char *current_cell = NULL;
    unsigned char *new_cell = NULL;
    int x = 0;
    int y = 0;
    int cell_x = 0;
    int cell_y = 0;
    int need_redraw;

    while (1) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }

        need_redraw = 0;
        if (event.type == SDL_MOUSEMOTION) {
            if (current_cell) {
                new_cell = current_cell;
                if (in_rect(event.motion.x, event.motion.y, &fieldRect)) {
                    x = (event.motion.x - fieldRect.x) / cell_size;
                    y = (event.motion.y - fieldRect.y) / cell_size;
                    new_cell = field + y * field_w + x;
                } else {
                    new_cell = NULL;
                }
                if (new_cell != current_cell) {
                    draw_panel(screenSurface,
                        fieldRect.x + cell_x * cell_size, fieldRect.y + cell_y * cell_size,
                        cell_size, cell_size, 2, COLOR_BODY, COLOR_LIGHT, COLOR_SHADOW, 0
                    );
                    current_cell = NULL;
                    need_redraw = 1;
                }
            }
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (in_rect(event.button.x, event.button.y, &fieldRect)) {
                cell_x = (event.button.x - fieldRect.x) / cell_size;
                cell_y = (event.button.y - fieldRect.y) / cell_size;
                current_cell = field + cell_y * field_w + cell_x;
                if (*current_cell & F_CLOSED) {
                    draw_panel(screenSurface,
                       fieldRect.x + cell_x * cell_size, fieldRect.y + cell_y * cell_size,
                       cell_size, cell_size, 1, COLOR_BODY, COLOR_SHADOW, COLOR_BODY, 1
                    );
                    need_redraw = 1;
                } else {
                    current_cell = NULL;
                }
            }
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (current_cell) {
                *current_cell &= ~F_CLOSED;
                current_cell = NULL;
                draw_field(screenSurface, imgSurface, field, field_w, field_h, cell_size);
                need_redraw = 1;
            }
        }
        if (need_redraw) {
            SDL_UpdateWindowSurface(window);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
