int in_rect(int x, int y, SDL_Rect *rect) {
    return x >= rect->x && y >= rect->y && x < rect->x + rect->w & y < rect->y + rect->h;
}

const char *get_option_title(int data) {
    if (data == MENUITEM_BEGINNER) {
        return "Beginner";
    } else if (data == MENUITEM_INTERMEDIATE) {
        return "Intermediate";
    }
    return "Expert";
}

MouseBox *process_menu_down(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    draw_menuitem_down(fieldView, box->rect, get_option_title(box->data));
    fieldView->need_redraw = 1;
    return box;
}

void pop_menuboxes(FieldView *fieldView) {
    pop_mousebox(fieldView, 3);
}

void process_menu_out(MouseBox *box, FieldView *fieldView) {
    draw_menuitem_up(fieldView, box->rect, get_option_title(box->data));
    fieldView->need_redraw = 1;
}

void start_game(FieldView *fieldView) {
    fieldView->game_mode = GAME_INIT;
    fieldView->currentFace = &field_images[SMILEY_HAPPY];
    fieldView->open_cells = 0;
    fieldView->explosion_addr = -1;
    fieldView->timer = 0;
    FieldModel *fieldModel = fieldView->fieldModel;
    init_field(
        fieldModel, fieldModel->width, fieldModel->height,
        fieldModel->mine_count
    );
    draw_field(fieldView);
}

void process_menu_up(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    process_menu_out(box, fieldView);
    pop_menuboxes(fieldView);
    if (box->data == MENUITEM_BEGINNER) {
        fieldView->fieldModel->width = 9;
        fieldView->fieldModel->height = 9;
        fieldView->fieldModel->mine_count = 10;
    } else if (box->data == MENUITEM_INTERMEDIATE) {
        fieldView->fieldModel->width = 16;
        fieldView->fieldModel->height = 16;
        fieldView->fieldModel->mine_count = 40;
    } else {
        fieldView->fieldModel->width = 30;
        fieldView->fieldModel->height = 16;
        fieldView->fieldModel->mine_count = 99;
    }
    set_view_sizes(fieldView);
    SDL_FreeSurface(fieldView->screenSurface);
    SDL_SetWindowSize(fieldView->window, fieldView->width, fieldView->height);
    fieldView->screenSurface = SDL_GetWindowSurface(fieldView->window);
    start_game(fieldView);
}

void process_menubutton_out(MouseBox *box, FieldView *fieldView) {
    draw_menubutton_up(fieldView);
    fieldView->need_redraw = 1;
}

void process_menubutton_up(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    if (fieldView->game_mode == GAME_MENU) {
        process_menubutton_out(box, fieldView);
        pop_menuboxes(fieldView);
        draw_field(fieldView);
        fieldView->game_mode = fieldView->old_game_mode;
    } else {
        process_menubutton_out(box, fieldView);
        push_mousebox(fieldView, &fieldView->menuItemBeginnerBox);
        push_mousebox(fieldView, &fieldView->menuItemIntermediateBox);
        push_mousebox(fieldView, &fieldView->menuItemExpertBox);
        fieldView->old_game_mode = fieldView->game_mode;
        fieldView->game_mode = GAME_MENU;
        draw_menu(fieldView);
    }
}

MouseBox *process_menubutton_down(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    draw_menubutton_down(fieldView);
    fieldView->need_redraw = 1;
    return box;
}

MouseBox *process_startbutton_down(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    draw_startbutton_down(fieldView);
    fieldView->need_redraw = 1;
    return box;
}

void process_startbutton_out(MouseBox *box, FieldView *fieldView) {
    SDL_Rect *rect = &fieldView->startButtonRect;
    draw_panel(
        fieldView->screenSurface, rect->x, rect->y, rect->w, rect->h, 2,
        fieldView->color_body, fieldView->color_light, fieldView->color_shadow, 1
    );
    draw_image(
        fieldView->screenSurface, fieldView->imageSurface, fieldView->currentFace,
        rect->x + 4, rect->y + 4
    );
    fieldView->need_redraw = 1;
}

void process_startbutton_up(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    process_startbutton_out(box, fieldView);
    if (fieldView->game_mode == GAME_MENU) {
        pop_menuboxes(fieldView);
    }
    FieldModel *fieldModel = fieldView->fieldModel;
    start_game(fieldView);
}

void process_field_out(MouseBox *box, FieldView *fieldView) {
    int cell_x = box->data % fieldView->fieldModel->width;
    int cell_y = box->data / fieldView->fieldModel->width;
    draw_closed_cell(fieldView, cell_x, cell_y);
    fieldView->need_redraw = 1;
}

MouseBox *process_field_move(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    int cell_x = (event->button.x - fieldView->fieldRect.x) / fieldView->cell_size;
    int cell_y = (event->button.y - fieldView->fieldRect.y) / fieldView->cell_size;
    int offset = cell_y * fieldView->fieldModel->width + cell_x;
    if (offset != box->data) {
        process_field_out(box, fieldView);
        return NULL;
    }
    return box;
}

MouseBox *process_field_down(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    if (fieldView->game_mode == GAME_STOP || fieldView->game_mode == GAME_MENU) {
        return NULL;
    }
    int cell_x = (event->button.x - fieldView->fieldRect.x) / fieldView->cell_size;
    int cell_y = (event->button.y - fieldView->fieldRect.y) / fieldView->cell_size;
    int offset = cell_y * fieldView->fieldModel->width + cell_x;
    unsigned char *f = fieldView->fieldModel->field + offset; 
    if (*f & F_CLOSED) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            if  (!(*f & (F_FLAG | F_QUESTION))) {
                draw_open_cell(fieldView, cell_x, cell_y);
                fieldView->need_redraw = 1;
                box->data = offset;
                return box;
            }
        } else {
            switch_flag(f);
            draw_field(fieldView);
        }
    }
    return NULL;
}

void *process_field_up(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    FieldModel *fieldModel = fieldView->fieldModel;
    int cell_x = box->data % fieldModel->width;
    int cell_y = box->data / fieldModel->width;
    if (fieldView->game_mode == GAME_INIT) {
        init_mines(fieldView->fieldModel, box->data);
        fieldView->game_mode = GAME_PLAY;
        fieldView->timer = 0;
        fieldView->start_ticks = SDL_GetTicks();
    }

    if (fieldModel->field[box->data] & F_MINE) {
        finalize_field(fieldModel);
        fieldView->explosion_addr = box->data;
        fieldView->game_mode = GAME_STOP;
        fieldView->currentFace = &field_images[SMILEY_DEAD];
    } else {
        fieldView->open_cells += open_cell(fieldModel, cell_x, cell_y);
        if (fieldView->open_cells == fieldModel->total - fieldModel->mine_count) {
            finalize_field_win(fieldModel);
            fieldView->game_mode = GAME_STOP;
            fieldView->currentFace = &field_images[SMILEY_COOL];
        }

    }
    draw_field(fieldView);
}

MouseBox *process_mouse_down(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    if (box->type == BOX_FIELD) {
        return process_field_down(box, event, fieldView);
    } else if (box->type == BOX_MENUITEM) {
        return process_menu_down(box, event, fieldView);
    } else if (box->type == BOX_STARTBUTTON) {
        return process_startbutton_down(box, event, fieldView);
    } else if (box->type == BOX_MENUBUTTON) {
        return process_menubutton_down(box, event, fieldView);
    }
}

void process_mouse_up(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    if (box->type == BOX_FIELD) {
        process_field_up(box, event, fieldView);
    } else if (box->type == BOX_MENUITEM) {
        process_menu_up(box, event, fieldView);
    } else if (box->type == BOX_STARTBUTTON) {
        process_startbutton_up(box, event, fieldView);
    } else if (box->type == BOX_MENUBUTTON) {
        process_menubutton_up(box, event, fieldView);
    }
}

void process_mouse_out(MouseBox *box, FieldView *fieldView) {
    if (box->type == BOX_FIELD) {
        process_field_out(box, fieldView);
    } else if (box->type == BOX_MENUITEM) {
        process_menu_out(box, fieldView);
    } else if (box->type == BOX_STARTBUTTON) {
        process_startbutton_out(box, fieldView);
    } else if (box->type == BOX_MENUBUTTON) {
        process_menubutton_out(box, fieldView);
    }
}

MouseBox *process_mouse_move(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    if (box->type == BOX_FIELD) {
        return process_field_move(box, event, fieldView);
    }
    return box;
}
