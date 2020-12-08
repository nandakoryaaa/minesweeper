void open_cell(FieldModel *fieldModel, int x, int y) {
    unsigned char *f = fieldModel->field + y * fieldModel->width + x;
    if (*f & F_CLOSED) {
        *f &= ~(F_CLOSED | F_FLAG | F_QUESTION);
        if (!*f & F_NUMBER) {
            if (y > 0) {
                if (x > 0) {
                    open_cell(fieldModel, x - 1, y - 1);
                }
                open_cell(fieldModel, x, y - 1);
                if (x < fieldModel->width - 1) {
                    open_cell(fieldModel, x + 1, y - 1);
                }
            }
            if (x > 0) {
                open_cell(fieldModel, x - 1, y);
            }
            if (x < fieldModel->width - 1) {
                open_cell(fieldModel, x + 1, y);
            }
            if (y < fieldModel->height - 1) {
                if (x > 0) {
                    open_cell(fieldModel, x - 1, y + 1);
                }
                open_cell(fieldModel, x, y + 1);
                if (x < fieldModel->width - 1) {
                    open_cell(fieldModel, x + 1, y + 1);
                }
            }
        }
    }
}

void switch_flag(unsigned char *f) {
    if (*f & F_FLAG) {
        *f = (*f & ~F_FLAG) | F_QUESTION;
    } else if (*f & F_QUESTION) {
        *f &= ~F_QUESTION;
    } else {
        *f |= F_FLAG;
    }
}

int init_field(FieldModel *fieldModel, int w, int h, int mine_count) {
    int total = w * h;

    if (total > MAX_FIELD_SIZE || mine_count > total) {
        return 0;
    }

    fieldModel->width = w;
    fieldModel->height = h;
    fieldModel->mine_count = mine_count;
    unsigned char *field = fieldModel->field;
    int i;

    for (i = 0; i < total; i++) {
        field[i] = F_CLOSED;
    }

    int mine_x, mine_y, mine_addr;

    for (i = 0; i < mine_count; i++) {
        do {
            mine_x = rnd(w);
            mine_y = rnd(h);
            mine_addr = mine_y * w + mine_x;
        } while (field[mine_addr] & F_MINE);

        field[mine_addr] |= F_MINE;

        if (mine_y > 0) {
            field[mine_addr - w]++;
            if (mine_x > 0) {
                field[mine_addr - w - 1]++;
            }
            if (mine_x < w - 1) {
                field[mine_addr - w + 1]++;
            }
        }
        if (mine_x > 0) {
            field[mine_addr - 1]++;
        }
        if (mine_x < w - 1) {
            field[mine_addr + 1]++;
        }
        if (mine_y < h - 1) {
            field[mine_addr + w]++;
            if (mine_x > 0) {
                field[mine_addr + w - 1]++;
            }
            if (mine_x < w - 1) {
                field[mine_addr + w + 1]++;
            }
        }
    }

    return 1;
}
