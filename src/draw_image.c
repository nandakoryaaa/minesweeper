void draw_image(SDL_Surface *surface, SDL_Surface *img_surface, SDL_Rect *img_rect, int x, int y) {
    SDL_Rect dst_rect = {x, y, img_rect->w, img_rect->h};
    SDL_LowerBlit(img_surface, img_rect, surface, &dst_rect);
}

void draw_digit(SDL_Surface *surface, SDL_Surface *img_surface, SDL_Rect *img_rect, int x, int y, int digit) {
    SDL_Rect dst_rect = {x, y, img_rect->w  img_rect->h};
    SDL_Rect src_rect = {img_rect->x + digit * img_rect->w, img_rect->y, img_rect->w, img_rect->h};
    SDL_LowerBlit(img_surface, &src_rect, surface, &dst_rect);
}

void draw_number(SDL_Surface *surface, SDL_Surface *img_surface, SDL_Rect *img_rect, int x, int y, int number) {
    draw_digit(surface, img_surface, img_rect, x, y, number / 100);
    draw_digit(surface, img_surface, img_rect, x + img_rect->w + 2, y, (number / 10) % 10);
    draw_digit(surface, img_surface, img_rect, x + img_rect->w * 2 + 4, y, number % 10);
}
