void set_view_sizes(FieldView *view) {
    view->width = view->fieldModel->width * view->cell_size + (3 + 6 + 3) * 2;
    view->height = view->fieldModel->height * view->cell_size + 3 + 6 + 3 + 3 + 49 + 3;
    view->fieldRect.x = 3 + 6 + 3;
    view->fieldRect.y = 3 + 49 + 3;
    view->fieldRect.w = view->fieldModel->width * view->cell_size;
    view->fieldRect.h = view->fieldModel->height * view->cell_size;
    view->startButtonRect.y = 15;
    view->startButtonRect.w = 25;
    view->startButtonRect.h = 25;
    view->menuButtonRect.y = 15;
    view->menuButtonRect.w = 16;
    view->menuButtonRect.h = 25;
    view->startButtonRect.x = (view->width - view->startButtonRect.w - view->menuButtonRect.w - 1) >> 1;
    view->menuButtonRect.x = view->startButtonRect.x + view->startButtonRect.w + 1;
}

int in_rect(int x, int y, SDL_Rect *rect) {
    return x >= rect->x && y >= rect->y && x < rect->x + rect->w & y < rect->y + rect->h;
}

void draw_image(SDL_Surface *surface, SDL_Surface *img_surface, SDL_Rect *img_rect, int x, int y) {
    SDL_Rect dst_rect = {x, y, img_rect->w, img_rect->h};
    SDL_LowerBlit(img_surface, img_rect, surface, &dst_rect);
}

void draw_digit(SDL_Surface *surface, SDL_Surface *img_surface, SDL_Rect *img_rect, int x, int y, int digit) {
    SDL_Rect dst_rect = {x, y, img_rect->w, img_rect->h};
    SDL_Rect src_rect = {img_rect->x + digit * img_rect->w, img_rect->y, img_rect->w, img_rect->h};
    SDL_LowerBlit(img_surface, &src_rect, surface, &dst_rect);
}

void draw_number(SDL_Surface *surface, SDL_Surface *img_surface, SDL_Rect *img_rect, int x, int y, int number) {
    draw_digit(surface, img_surface, img_rect, x, y, number / 100);
    draw_digit(surface, img_surface, img_rect, x + img_rect->w + 2, y, (number / 10) % 10);
    draw_digit(surface, img_surface, img_rect, x + img_rect->w * 2 + 4, y, number % 10);
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

void draw_closed_cell(FieldView *fieldView, int cell_x, int cell_y) {
    int cell_size = fieldView->cell_size;
    draw_panel(
        fieldView->screenSurface,
        fieldView->fieldRect.x + cell_x * cell_size,
        fieldView->fieldRect.y + cell_y * cell_size,
        cell_size, cell_size, 2, fieldView->color_body, fieldView->color_light, fieldView->color_shadow, 0
    );
    draw_image(
        fieldView->screenSurface, fieldView->imageSurface, fieldView->currentFace,
        fieldView->startButtonRect.x + 4, fieldView->startButtonRect.y + 4
    );
    fieldView->need_redraw = 1;
}

void draw_open_cell(FieldView *fieldView, int cell_x, int cell_y) {
    int cell_size = fieldView->cell_size;
    draw_panel(
        fieldView->screenSurface,
        fieldView->fieldRect.x + cell_x * cell_size,
        fieldView->fieldRect.y + cell_y * cell_size,
        cell_size, cell_size, 1, fieldView->color_body, fieldView->color_shadow, fieldView->color_body, 1
    );
    draw_image(
        fieldView->screenSurface, fieldView->imageSurface, &field_images[SMILEY_SCARED],
        fieldView->startButtonRect.x + 4, fieldView->startButtonRect.y + 4
    );
    fieldView->need_redraw = 1;
}

void draw_field(FieldView *fieldView) {
    int x, y;
    int addr = 0;
    int cell_x;
    int cell_y;
    unsigned char f;
    SDL_Surface *screenSurface = fieldView->screenSurface;
    SDL_Surface *imageSurface = fieldView->imageSurface;
    int view_w = fieldView->width;
    int view_h = fieldView->height;

    draw_panel(screenSurface, 0, 0, view_w, view_h, 3, COLOR_BODY, COLOR_LIGHT, COLOR_SHADOW, 1);
    draw_panel(screenSurface, 9, 52, view_w - 18, view_h - 9 - 52, 3, 0, COLOR_SHADOW, COLOR_LIGHT, 0);
    draw_panel(screenSurface, 9, 9, view_w - 18, 37, 2, 0, COLOR_SHADOW, COLOR_LIGHT, 0);
    draw_panel(screenSurface, 16, 15, 41, 25, 1, 0, COLOR_SHADOW, COLOR_LIGHT, 1);
    draw_panel(screenSurface, view_w - 16 - 41, 15, 41, 25, 1, 0, COLOR_SHADOW, COLOR_LIGHT, 1);

    SDL_Rect *rect = &fieldView->startButtonRect;
    draw_panel(screenSurface, rect->x, rect->y, rect->w, rect->h, 2, 0, COLOR_LIGHT, COLOR_SHADOW, 0);
    draw_panel(screenSurface, rect->x - 1, rect->y - 1, rect->w + 2, rect->h + 2, 1, 0, COLOR_SHADOW, COLOR_SHADOW, 0);
    draw_image(screenSurface, imageSurface, fieldView->currentFace, rect->x + 4, rect->y + 4);

    rect = &fieldView->menuButtonRect;
    draw_panel(screenSurface, rect->x, rect->y, rect->w, rect->h, 2, 0, COLOR_LIGHT, COLOR_SHADOW, 0);
    draw_panel(screenSurface, rect->x - 1, rect->y - 1, rect->w + 2, rect->h + 2, 1, 0, COLOR_SHADOW, COLOR_SHADOW, 0);
    draw_image(screenSurface, imageSurface, &field_images[MENU_OPEN], rect->x + 3, rect->y + 11);

    int flagged_mines = 0;
    int cell_size = fieldView->cell_size;
    FieldModel *fieldModel = fieldView->fieldModel;
    
    for (y = 0; y < fieldModel->height; y++) {
        for (x = 0; x < fieldModel->width; x++) {
            f = fieldModel->field[addr++];
            cell_x = fieldView->fieldRect.x + x * cell_size;
            cell_y = fieldView->fieldRect.y + y * cell_size;
            if (f & F_CLOSED) {
                draw_panel(screenSurface, cell_x, cell_y, cell_size, cell_size, 2, COLOR_BODY, COLOR_LIGHT, COLOR_SHADOW, 0);
                if (f & F_FLAG) {
                    draw_image(screenSurface, imageSurface, &field_images[ICON_FLAG], cell_x + 3, cell_y + 3);
                    flagged_mines++;
                } else if (f & F_QUESTION) {
                    draw_image(screenSurface, imageSurface, &field_images[ICON_QUESTION], cell_x + 3, cell_y + 3);
                }
            } else {
                draw_panel(screenSurface, cell_x, cell_y, cell_size, cell_size, 1, COLOR_BODY, COLOR_SHADOW, COLOR_BODY, 1);
                if (f & F_MINE) {
                    draw_image(screenSurface, imageSurface, &field_images[ICON_MINE], cell_x + 1, cell_y + 1);
                } else {
                    int mine_count = f & F_NUMBER;
                    if (mine_count) {
                        draw_digit(screenSurface, imageSurface, &field_digits, cell_x + 3, cell_y + 3, mine_count - 1);
                    }
                }
            }
        }
    }

    draw_number(screenSurface, imageSurface, &red_digits, 18, 17, fieldModel->mine_count - flagged_mines);
    draw_number(screenSurface, imageSurface, &red_digits, view_w - 55, 17, 123);
    fieldView->need_redraw = 1;
}
