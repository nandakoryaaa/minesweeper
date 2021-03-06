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
    int item_x = view->fieldRect.x + 6;
    int item_y = view->fieldRect.y + 6;
    int item_w = view->fieldRect.w - 12;
    view->menuItemBeginnerRect.x = item_x;
    view->menuItemBeginnerRect.w = item_w;
    view->menuItemBeginnerRect.y = item_y;
    view->menuItemBeginnerRect.h = 24;
    view->menuItemIntermediateRect.x = item_x;
    view->menuItemIntermediateRect.w = item_w;
    view->menuItemIntermediateRect.y = item_y + 32;
    view->menuItemIntermediateRect.h = 24;
    view->menuItemExpertRect.x = item_x;
    view->menuItemExpertRect.w = item_w;
    view->menuItemExpertRect.y = item_y + 64;
    view->menuItemExpertRect.h = 24;
}

void draw_text(SDL_Surface * surface, int x, int y, const char *str, int color) {
    unsigned char *c = (unsigned char *)str;
    int i;
    Uint32 col = SDL_MapRGB(
        surface->format,
        (color >> 16) & 255, (color >> 8) & 255, color & 255
    );

    SDL_Rect rect = {0, 0, 1, 1};
    while(*c) {
        int letter = 2 + (*c - 1) * 18;
        rect.y = y;
        for (i = 0; i < 16; i++) {
            rect.x = x + 7;
            unsigned char byte = font08x16[letter++];
            while(byte) {
                if (byte & 1) {
		            SDL_FillRect(surface, &rect, col);
                }
                byte >>= 1;
                rect.x--;
            }
            rect.y++;
        }
        x += 8;
        c++;
    }
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
}

void draw_menuitem_up(FieldView *fieldView, SDL_Rect *rect, const char *s) {
    draw_panel(
        fieldView->screenSurface, rect->x, rect->y, rect->w, rect->h, 2,
        fieldView->color_body, fieldView->color_light, fieldView->color_shadow, 1
    );
    draw_panel(
        fieldView->screenSurface, rect->x - 1, rect->y - 1, rect->w + 2, rect->h + 2, 1,
        0, fieldView->color_shadow, fieldView->color_shadow, 0
    );
    draw_text(fieldView->screenSurface, rect->x + 10, rect->y + 5, s, 0);
}

void draw_menuitem_down(FieldView *fieldView, SDL_Rect *rect, const char *s) {
    draw_panel(
        fieldView->screenSurface, rect->x, rect->y, rect->w + 1, rect->h + 1, 1,
        fieldView->color_body, fieldView->color_shadow, fieldView->color_shadow, 1
    );
    draw_text(fieldView->screenSurface, rect->x + 11, rect->y + 6, s, 0);
}

void draw_menu(FieldView *fieldView) {
    draw_panel(
        fieldView->screenSurface, fieldView->fieldRect.x, fieldView->fieldRect.y,
        fieldView->fieldRect.w, fieldView->fieldRect.h, 0,
        fieldView->color_body, 0, 0, 1
    );

    draw_menuitem_up(fieldView, &fieldView->menuItemBeginnerRect, "Beginner");
    draw_menuitem_up(fieldView, &fieldView->menuItemIntermediateRect, "Intermediate");
    draw_menuitem_up(fieldView, &fieldView->menuItemExpertRect, "Expert");
}

void draw_startbutton_up(FieldView *fieldView) {
    SDL_Rect *rect = &fieldView->startButtonRect;
    draw_panel(
        fieldView->screenSurface, rect->x - 1, rect->y - 1, rect->w + 2, rect->h + 2, 1,
        0, fieldView->color_shadow, fieldView->color_shadow, 0
    );
    draw_panel(
        fieldView->screenSurface, rect->x, rect->y, rect->w, rect->h, 2,
        fieldView->color_body, fieldView->color_light, fieldView->color_shadow, 1
    );
    draw_image(
        fieldView->screenSurface, fieldView->imageSurface, fieldView->currentFace,
        rect->x + 4, rect->y + 4
    );
}

void draw_startbutton_down(FieldView *fieldView) {
    SDL_Rect *rect = &fieldView->startButtonRect;
    draw_panel(
        fieldView->screenSurface, rect->x, rect->y, rect->w + 1, rect->h + 1, 1,
        fieldView->color_body, fieldView->color_shadow, fieldView->color_shadow, 1
    );
    draw_image(
        fieldView->screenSurface, fieldView->imageSurface, fieldView->currentFace,
        rect->x + 5, rect->y + 5
    );
}

void draw_menubutton_up(FieldView *fieldView) {
    SDL_Rect *rect = &fieldView->menuButtonRect;
    draw_panel(
        fieldView->screenSurface, rect->x - 1, rect->y - 1, rect->w + 2, rect->h + 2, 1,
        0, fieldView->color_shadow, fieldView->color_shadow, 0
    );
    draw_panel(
        fieldView->screenSurface, rect->x, rect->y, rect->w, rect->h, 2,
        fieldView->color_body, fieldView->color_light, fieldView->color_shadow, 1
    );
    SDL_Rect *sign;
    if (fieldView->game_mode == GAME_MENU) {
        sign = &field_images[MENU_CLOSE];
    } else {
        sign = &field_images[MENU_OPEN];
    }
    draw_image(
        fieldView->screenSurface, fieldView->imageSurface,
        sign, rect->x + 3, rect->y + 11
    );
}

void draw_menubutton_down(FieldView *fieldView) {
    SDL_Rect *rect = &fieldView->menuButtonRect;
    draw_panel(
        fieldView->screenSurface, rect->x, rect->y, rect->w + 1, rect->h + 1, 1,
        fieldView->color_body, fieldView->color_shadow, fieldView->color_shadow, 1
    );
    SDL_Rect *sign;
    if (fieldView->game_mode == GAME_MENU) {
        sign = &field_images[MENU_CLOSE];
    } else {
        sign = &field_images[MENU_OPEN];
    }
    draw_image(
        fieldView->screenSurface, fieldView->imageSurface,
        sign, rect->x + 4, rect->y + 12
    );
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

    draw_panel(screenSurface, 0, 0, view_w, view_h, 3, fieldView->color_body, fieldView->color_light, fieldView->color_shadow, 1);
    draw_panel(screenSurface, 9, 52, view_w - 18, view_h - 9 - 52, 3, 0, fieldView->color_shadow, fieldView->color_light, 0);
    draw_panel(screenSurface, 9, 9, view_w - 18, 37, 2, 0, fieldView->color_shadow, fieldView->color_light, 0);
    draw_panel(screenSurface, 16, 15, 41, 25, 1, 0, fieldView->color_shadow, fieldView->color_light, 1);
    draw_panel(screenSurface, view_w - 16 - 41, 15, 41, 25, 1, 0, fieldView->color_shadow, fieldView->color_light, 1);
  
    draw_startbutton_up(fieldView);
    draw_menubutton_up(fieldView);

    int flagged_mines = 0;
    int cell_size = fieldView->cell_size;
    int half_5 = (cell_size >> 1) - 5;
    int half_7 = half_5 - 2;
    FieldModel *fieldModel = fieldView->fieldModel;
    
    for (y = 0; y < fieldModel->height; y++) {
        for (x = 0; x < fieldModel->width; x++) {
            f = fieldModel->field[addr];
            cell_x = fieldView->fieldRect.x + x * cell_size;
            cell_y = fieldView->fieldRect.y + y * cell_size;
            if (f & F_CLOSED) {
                draw_panel(
                    screenSurface, cell_x, cell_y, cell_size, cell_size, 2,
                    fieldView->color_body, fieldView->color_light, fieldView->color_shadow, 0
                );
                if (f & F_FLAG) {
                    draw_image(screenSurface, imageSurface, &field_images[ICON_FLAG], cell_x + half_5, cell_y + half_5);
                    flagged_mines++;
                } else if (f & F_QUESTION) {
                    draw_image(screenSurface, imageSurface, &field_images[ICON_QUESTION], cell_x + half_5, cell_y + half_5);
                }
            } else {
                if (addr == fieldView->explosion_addr) {
                    draw_panel(
                        screenSurface, cell_x, cell_y, cell_size, cell_size, 0,
                        fieldView->color_explosion, 0, 0, 1
                    );
                    draw_image(screenSurface, imageSurface, &field_images[ICON_RED_MINE], cell_x + half_7, cell_y + half_7);
                } else {
                    draw_panel(
                        screenSurface, cell_x, cell_y, cell_size, cell_size, 1,
                        fieldView->color_body, fieldView->color_shadow, fieldView->color_body, 1
                    );
                
                    if (f & F_MINE) {
                        draw_image(screenSurface, imageSurface, &field_images[ICON_MINE], cell_x + half_7, cell_y + half_7);
                    } else if (f & F_FLAG) {
                        draw_image(screenSurface, imageSurface, &field_images[ICON_ERROR], cell_x + half_7, cell_y + half_7);
                    } else {
                        int mine_count = f & F_NUMBER;
                        if (mine_count) {
                            draw_digit(screenSurface, imageSurface, &field_digits, cell_x + half_5, cell_y + half_5, mine_count - 1);
                        }
                    }
                }
            }
            addr++;
        }
    }

    draw_number(screenSurface, imageSurface, &red_digits, 18, 17, fieldModel->mine_count - flagged_mines);
    draw_number(screenSurface, imageSurface, &red_digits, view_w - 55, 17, fieldView->timer);
    fieldView->need_redraw = 1;
}

void update_timer(FieldView *fieldView) {
    if (fieldView->timer > 998) {
        return;
    }
    int ticks = SDL_GetTicks();
    int diff = ticks - fieldView->start_ticks;
    if (diff > 999) {
        fieldView->timer++;
        fieldView->start_ticks = ticks - (diff - 1000);
        draw_number(
            fieldView->screenSurface, fieldView->imageSurface,
            &red_digits, fieldView->width - 55, 17, fieldView->timer
        );
        fieldView->need_redraw = 1;
    }
}
