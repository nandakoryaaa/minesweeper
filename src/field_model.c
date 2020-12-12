int open_cell(FieldModel *fieldModel, int x, int y) {
    int count = 0;
    unsigned char *f = fieldModel->field + y * fieldModel->width + x;
    if (*f & F_CLOSED) {
        count++;
        *f &= ~(F_CLOSED | F_FLAG | F_QUESTION);
        if (!*f & F_NUMBER) {
            if (y > 0) {
                if (x > 0) {
                    count += open_cell(fieldModel, x - 1, y - 1);
                }
                count += open_cell(fieldModel, x, y - 1);
                if (x < fieldModel->width - 1) {
                    count += open_cell(fieldModel, x + 1, y - 1);
                }
            }
            if (x > 0) {
                count += open_cell(fieldModel, x - 1, y);
            }
            if (x < fieldModel->width - 1) {
                count += open_cell(fieldModel, x + 1, y);
            }
            if (y < fieldModel->height - 1) {
                if (x > 0) {
                    count += open_cell(fieldModel, x - 1, y + 1);
                }
                count += open_cell(fieldModel, x, y + 1);
                if (x < fieldModel->width - 1) {
                    count += open_cell(fieldModel, x + 1, y + 1);
                }
            }
        }
    }

    return count;
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

void finalize_field(FieldModel *fieldModel) {
    int total = fieldModel->total;
    unsigned char *f = fieldModel->field;
    while(total--) {
        *f &= ~F_QUESTION;
        if (*f & F_MINE && !(*f & F_FLAG)) {
            *f &= ~F_CLOSED;
        } else if (!(*f & F_MINE) && *f & F_FLAG) {
            *f &= ~F_CLOSED;
        }
        f++;
    }
}

void finalize_field_win(FieldModel *fieldModel) {
    int total = fieldModel->total;
    unsigned char *f = fieldModel->field;
    while(total--) {
        *f &= ~F_QUESTION;
        if (*f & F_MINE && !(*f & F_FLAG)) {
            *f |= F_FLAG;
        }
        f++;
    }
}

int init_field(FieldModel *fieldModel, int w, int h, int mine_count) {
    int total = w * h;

    if (total > MAX_FIELD_SIZE || mine_count > total) {
        return 0;
    }

    fieldModel->width = w;
    fieldModel->height = h;
    fieldModel->total = total;
    fieldModel->mine_count = mine_count;
    unsigned char *field = fieldModel->field;
    int i;

    for (i = 0; i < total; i++) {
        field[i] = F_CLOSED;
    }
}

void init_mines(FieldModel *fieldModel, int safe_addr) {
    int mine_x, mine_y, mine_addr;
    int w = fieldModel->width;
    int h = fieldModel->height;
    unsigned char *field = fieldModel->field;
    int i;

    for (i = 0; i < fieldModel->mine_count; i++) {
        do {
            mine_x = rnd(w);
            mine_y = rnd(h);
            mine_addr = mine_y * w + mine_x;
        } while (mine_addr == safe_addr || field[mine_addr] & F_MINE);

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
}
