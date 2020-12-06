void draw_field(
    SDL_Surface *surface,
    SDL_Surface *imgSurface,
    unsigned char *field,
    int width,
    int height,
    int cell_size
) {
    int x, y;
    int addr = 0;
    int cell_x;
    int cell_y;
    unsigned char f;
    SDL_Rect winRect;

    set_window_rect(width, height, cell_size, &winRect);
    int center_x = winRect.w >> 1;

    draw_panel(surface, 0, 0, winRect.w, winRect.h, 3, COLOR_BODY, COLOR_LIGHT, COLOR_SHADOW, 1);
    draw_panel(surface, 9, 52, winRect.w - 18, winRect.h - 9 - 52, 3, 0, COLOR_SHADOW, COLOR_LIGHT, 0);
    draw_panel(surface, 9, 9, winRect.w - 18, 37, 2, 0, COLOR_SHADOW, COLOR_LIGHT, 0);
    draw_panel(surface, 16, 15, 41, 25, 1, 0, COLOR_SHADOW, COLOR_LIGHT, 1);
    draw_panel(surface, winRect.w - 16 - 41, 15, 41, 25, 1, 0, COLOR_SHADOW, COLOR_LIGHT, 1);
    draw_panel(surface, center_x - 12, 15, 24, 24, 2, 0, COLOR_LIGHT, COLOR_SHADOW, 0);
    draw_panel(surface, center_x - 13, 14, 26, 26, 1, 0, COLOR_SHADOW, COLOR_SHADOW, 0);

    draw_image(surface, imgSurface, &smileys[SMILEY_HAPPY], center_x - 9, 18);

    int mine_count = 0;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            f = field[addr++];
            cell_x = 12 + x * cell_size;
            cell_y = 55 + y * cell_size;
            if (f & F_CLOSED) {
		draw_panel(surface, cell_x, cell_y, cell_size, cell_size, 2, COLOR_BODY, COLOR_LIGHT, COLOR_SHADOW, 0);
            } else {
                draw_panel(surface, cell_x, cell_y, cell_size, cell_size, 1, COLOR_BODY, COLOR_SHADOW, COLOR_BODY, 1);
                if (f & F_MINE) {
                    draw_image(surface, imgSurface, &field_icons[ICON_MINE], cell_x + 1, cell_y + 1);
                    mine_count++;
                } else {
                    int mine_count = f & F_NUMBER;
                    if (mine_count) {
                        draw_digit(surface, imgSurface, &field_digits, cell_x + 3, cell_y + 3, mine_count - 1);
                    }
                }
            }
        }
    }

    draw_number(surface, imgSurface, &red_digits, 18, 17, mine_count);
    draw_number(surface, imgSurface, &red_digits, winRect.w - 55, 17, 123);
}
