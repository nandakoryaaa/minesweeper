#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "icons.h"

#define MAX_FIELD_SIZE 10000
#define F_QUESTION     0b10000000
#define F_FLAG         0b01000000
#define F_CLOSED       0b00100000
#define F_MINE         0b00010000
#define F_NUMBER       0b00001111

unsigned char field[MAX_FIELD_SIZE];

int rnd(int max) {
    return rand() % max;
}

void draw_panel(
    SDL_Surface *surface,
    int x, int y, int width, int height, int frame_width,
    int body_color, int light_color, int shadow_color, int draw_body
) {
    light_color = SDL_MapRGB(
        surface->format,
        (light_color >> 16) & 255, (light_color >> 8) & 255, light_color & 255
    );

    shadow_color = SDL_MapRGB(
        surface->format,
        (shadow_color >> 16) & 255, (shadow_color >> 8) & 255, shadow_color & 255
    );

    SDL_Rect rect = {x, y, frame_width, height};
    SDL_FillRect(surface, &rect, light_color);

    rect.x += width - frame_width;
    SDL_FillRect(surface, &rect, shadow_color);

    rect.x = x + frame_width;
    rect.w = width - frame_width;
    rect.h = 1;
    int i;

    for (i = 0; i < frame_width; i++) {
        SDL_FillRect(surface, &rect, light_color);
        rect.y++;
        rect.w--;
    }

    rect.x = x + frame_width;
    rect.y = y + height - frame_width;
    rect.w = width - frame_width;

    for (i = 0; i < frame_width; i++) {
        SDL_FillRect(surface, &rect, shadow_color);
        rect.x--;
        rect.y++;
        rect.w++;
    }

    if (draw_body) {
        rect.x = x + frame_width;
        rect.y = y + frame_width;
        rect.w = width - frame_width * 2;
        rect.h = height - frame_width * 2;
        body_color = SDL_MapRGB(
            surface->format,
            body_color & 255, (body_color >> 8) & 255, (body_color >> 16) & 255
        );
        SDL_FillRect(surface, &rect, body_color);
    }
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

void print_field(unsigned char field[], int width, int height) {
    int addr = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (field[addr++] & F_MINE) {
                fputs("*", stdout);
            } else {
                int num_mines = field[addr] & F_NUMBER;
                if (!num_mines) {
                    fputs(".", stdout);
                } else {
                    printf("%d", num_mines);
                }
            }
        }
        puts("");
    }
}

void draw_field(
    SDL_Surface *surface,
    unsigned char *field,
    int width,
    int height
) {
    int x, y;
    int addr = 0;
    unsigned char f;

    int body_color = 0x808080;
    int light_color = 0xCCCCCC;
    int shadow_color = 0x333333;

    draw_panel(surface, 0, 0, 640, 480, 4, body_color, light_color, shadow_color, 1);
    draw_panel(surface, 8, 8, 640 - 16, 48, 4, 0, shadow_color, light_color, 0);
    draw_panel(surface, 8, 60, 640 - 16, 480 - 68, 4, 0, shadow_color, light_color, 0);
    draw_panel(surface, 24, 16, 128, 32, 1, 0, shadow_color, light_color, 1);
    draw_panel(surface, 640-24-128, 16, 128, 32, 1, 0, shadow_color, light_color, 1);
    draw_panel(surface, 320-16, 16, 32, 32, 4, 0, light_color, shadow_color, 0);

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            f = field[addr++];
            if (f & F_MINE) {
                draw_panel(surface, 12 + x * 16, 16 + 48 + y * 16, 16, 16, 2, body_color, light_color, shadow_color, 0); 
            } else {
                draw_panel(surface, 12 + x * 16, 16 + 48 + y * 16, 16, 16, 1, body_color, shadow_color, body_color, 1);
            }
        }
    }
}

int main(int argc, char* argv[]) {

    srand(time(0));

    int width = 38;
    int height = 25;
    init_field(field, width, height, (int) width * height / 6);
    print_field(field, width, height);

    SDL_Window *window;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "Minesweeper",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_OPENGL
    );

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
    draw_field(screenSurface, field, width, height);
    SDL_Surface *tmpSurface = SDL_CreateRGBSurfaceFrom(icons, 178, 32, 24, 536, 255 << 16, 255 << 8, 255, 0);
    SDL_Surface *iconsSurface = SDL_ConvertSurface(tmpSurface, screenSurface->format, 0);
    SDL_Rect iconRect = {0,0,178,32};
    SDL_LowerBlit(iconsSurface, &iconRect, screenSurface, &iconRect);


    SDL_UpdateWindowSurface(window);

    while (1) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
