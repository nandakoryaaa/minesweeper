int in_rect(int x, int y, SDL_Rect *rect) {
    return x >= rect->x && y >= rect->y && x < rect->x + rect->w & y < rect->y + rect->h;
}

MouseBox *process_menu_down(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    const char *s;
    if (box->data == MENUITEM_BEGINNER) {
        s = "Beginner";
    } else if (box->data == MENUITEM_INTERMEDIATE) {
        s = "Intermediate";
    } else {
        s = "Expert";
    }

    draw_menuitem_down(fieldView, box->rect, s);
    
    fieldView->need_redraw = 1;
    return box;
}

void pop_menuboxes(FieldView *fieldView) {
    pop_mousebox(fieldView, 3);
    push_mousebox(fieldView, &fieldView->fieldBox);
}

void process_menu_out(MouseBox *box, FieldView *fieldView) {
    const char *s;
    if (box->data == MENUITEM_BEGINNER) {
        s = "Beginner";
    } else if (box->data == MENUITEM_INTERMEDIATE) {
        s = "Intermediate";
    } else {
        s = "Expert";
    }

    draw_menuitem_up(fieldView, box->rect, s);
    fieldView->need_redraw = 1;
}

void process_menu_up(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    process_menu_out(box, fieldView);
    pop_menuboxes(fieldView);
    if (box->data == MENUITEM_BEGINNER) {
        init_field(fieldView->fieldModel, 9, 9, 10);
    } else if (box->data == MENUITEM_INTERMEDIATE) {
        init_field(fieldView->fieldModel, 16, 16, 40);
    } else {
        init_field(fieldView->fieldModel, 30, 16, 99);
    }
    set_view_sizes(fieldView);
    SDL_FreeSurface(fieldView->screenSurface);
    SDL_SetWindowSize(fieldView->window, fieldView->width, fieldView->height);
    fieldView->screenSurface = SDL_GetWindowSurface(fieldView->window);
    fieldView->game_mode = GAME_INIT;
    draw_field(fieldView);
}

void process_menubutton_out(MouseBox *box, FieldView *fieldView) {
    draw_menubutton_up(fieldView);
    fieldView->need_redraw = 1;
}

void process_menubutton_up(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    if (fieldView->game_mode == GAME_MENU) {
        pop_menuboxes(fieldView);
        draw_field(fieldView);
        fieldView->game_mode = fieldView->old_game_mode;
        process_menubutton_out(box, fieldView);
    } else {
        pop_mousebox(fieldView, 1);
        push_mousebox(fieldView, &fieldView->menuItemBeginnerBox);
        push_mousebox(fieldView, &fieldView->menuItemIntermediateBox);
        push_mousebox(fieldView, &fieldView->menuItemExpertBox);
        fieldView->old_game_mode = fieldView->game_mode;
        fieldView->game_mode = GAME_MENU;
        process_menubutton_out(box, fieldView);
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
    init_field(
        fieldModel, fieldModel->width, fieldModel->height,
        fieldModel->mine_count
    );
    fieldView->game_mode = GAME_INIT;
    draw_field(fieldView);
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
    int cell_x = box->data % fieldView->fieldModel->width;
    int cell_y = box->data / fieldView->fieldModel->width;
    open_cell(fieldView->fieldModel, cell_x, cell_y);
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
