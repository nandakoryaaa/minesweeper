#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <stdio.h>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        puts("usage: image_convert.exe filename");
	return 1;
    }

    char *filename = argv[1];

    SDL_Surface *surface = SDL_LoadBMP(filename);
    SDL_PixelFormat *format = surface->format;
    printf("bits per pixel: %d, bytes per pixel: %d\n", format->BitsPerPixel, format->BytesPerPixel);
    printf("rmask: %d, gmask: %d, bmask: %d\n", format->Rmask, format->Gmask, format->Bmask);
    printf("w: %d, h: %d, pitch: %d\n", surface->w, surface->h, surface->pitch);

    char *addr = (char *)surface->pixels;
    int length = surface->pitch * surface->h;
    int count = 0;
    for (int i = 0; i < length; i++) {
        printf("%d,", (unsigned char)*addr++);
        count++;
        if (count > 40) {
            count = 0;
            puts("");
        }
    }

    return 0;
}
