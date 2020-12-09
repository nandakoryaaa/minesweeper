MouseBox *process_menu_down(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    return NULL;
}


MouseBox *process_menu_move(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    return NULL;
}

void process_menu_out(MouseBox *box, FieldView *fieldView) {
}

void process_menu_up(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    process_menu_out(box, fieldView);
}

void process_menubutton_out(MouseBox *box, FieldView *fieldView) {
    SDL_Rect *rect = &fieldView->menuButtonRect;
    draw_panel(
        fieldView->screenSurface, rect->x, rect->y, rect->w, rect->h, 2,
        fieldView->color_body, fieldView->color_light, fieldView->color_shadow, 1
    );
    draw_image(
        fieldView->screenSurface, fieldView->imageSurface,
        &field_images[MENU_OPEN], rect->x + 3, rect->y + 11
    );
    fieldView->need_redraw = 1;
}

void process_menubutton_up(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    process_menubutton_out(box, fieldView);
}

MouseBox *process_menubutton_down(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    SDL_Rect *rect = &fieldView->menuButtonRect;
    draw_panel(
        fieldView->screenSurface, rect->x, rect->y, rect->w + 1, rect->h + 1, 1,
        fieldView->color_body, fieldView->color_shadow, fieldView->color_shadow, 1
    );
    draw_image(
        fieldView->screenSurface, fieldView->imageSurface,
        &field_images[MENU_OPEN], rect->x + 4, rect->y + 12
    );
    fieldView->need_redraw = 1;
    return box;
}

MouseBox *process_startbutton_down(MouseBox *box, SDL_Event *event, FieldView *fieldView) {
    SDL_Rect *rect = &fieldView->startButtonRect;
    draw_panel(
        fieldView->screenSurface, rect->x, rect->y, rect->w + 1, rect->h + 1, 1,
        fieldView->color_body, fieldView->color_shadow, fieldView->color_shadow, 1
    );
    draw_image(
        fieldView->screenSurface, fieldView->imageSurface, fieldView->currentFace,
        rect->x + 5, rect->y + 5
    );
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
    FieldModel *fieldModel = fieldView->fieldModel;
    init_field(
        fieldModel, fieldModel->width, fieldModel->height,
        fieldModel->width * fieldModel->height / 6
    );
    draw_field(fieldView);
}

void process_field_out(MouseBox *box, FieldView *fieldView) {
    int cell_x = box->data % fieldView->fieldModel->width;
    int cell_y = box->data / fieldView->fieldModel->width;
    draw_closed_cell(fieldView, cell_x, cell_y);
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
    } else if (box->type == BOX_MENU) {
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
    } else if (box->type == BOX_MENU) {
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
    } else if (box->type == BOX_MENU) {
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
    } else if (box->type == BOX_MENU) {
        return process_menu_move(box, event, fieldView);
    }
    return box;
}
