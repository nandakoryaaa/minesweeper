#define MAX_FIELD_SIZE 10000
#define F_QUESTION     0b10000000
#define F_FLAG         0b01000000
#define F_CLOSED       0b00100000
#define F_MINE         0b00010000
#define F_NUMBER       0b00001111

#define COLOR_BODY   0xc0c0c0
#define COLOR_LIGHT  0xffffff
#define COLOR_SHADOW 0x808080

#define MODE_PLAY 0
#define MODE_FINISH 1
#define MODE_MENU 2

#define ICON_QUESTION 0
#define ICON_FLAG 1
#define ICON_MINE 2
#define ICON_RED_MINE 3
#define ICON_ERROR 4

#define SMILEY_HAPPY 0
#define SMILEY_SCARED 1
#define SMILEY_DEAD 2
#define SMILEY_COOL 3

void set_window_rect(int w, int h, int cell_size, SDL_Rect *rect);
void draw_field(SDL_Surface *surface, SDL_Surface *imgSurface, unsigned char *field, int width, int height, int cell_size);
void draw_panel(SDL_Surface *surface, int x, int y, int width, int height, int frame_width, int body_color, int light_color, int shadow_color, int draw_body);

SDL_Rect red_digits = {0,0,11,21};
SDL_Rect field_digits = {20,21,10,10};

SDL_Rect field_icons[] = {
    {0,21,10,10}, {10,21,10,10}, {110,17,15,15},
    {125,17,15,15}, {140,17,15,15}
};

SDL_Rect smileys[] = {
    {110,0,17,17}, {127,0,17,17},
    {144,0,17,17}, {161,0,17,17}
};
