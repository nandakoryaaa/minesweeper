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
