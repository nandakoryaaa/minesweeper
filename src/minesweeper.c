#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    SDL_Surface *surface, unsigned char field[], int width, int height
) {
    SDL_Rect rect = {0, 0, 15, 15};
    int x, y;
    int addr = 0;
    unsigned char f;
    Uint32 field_color = SDL_MapRGB(surface->format, 192, 192, 192);
    Uint32 mine_color = SDL_MapRGB(surface->format, 0, 0, 0);
    Uint32 color;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            f = field[addr];
            if (f & F_MINE) {
                color = mine_color;
            } else {
                color = field_color;
            }
            rect.x = 10 + x * 16;
            rect.y = 10 + y * 16;

            SDL_FillRect(surface, &rect, color);
            addr++;
        }
    }
}

int main(int argc, char* argv[]) {

    srand(time(0));

    int width = 25;
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
