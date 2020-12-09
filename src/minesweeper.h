#define MAX_FIELD_SIZE 10000
#define F_QUESTION     0b10000000
#define F_FLAG         0b01000000
#define F_CLOSED       0b00100000
#define F_MINE         0b00010000
#define F_NUMBER       0b00001111

#define GAME_INIT   0
#define GAME_PLAY   1
#define GAME_STOP   2
#define GAME_MENU   3

#define COLOR_BODY   0xc0c0c0
#define COLOR_LIGHT  0xffffff
#define COLOR_SHADOW 0x808080

#define ICON_QUESTION 0
#define ICON_FLAG 1
#define ICON_MINE 2
#define ICON_RED_MINE 3
#define ICON_ERROR 4
#define SMILEY_HAPPY 5
#define SMILEY_SCARED 6
#define SMILEY_DEAD 7
#define SMILEY_COOL 8
#define MENU_OPEN 9
#define MENU_CLOSE 10

#define BOX_STARTBUTTON 0
#define BOX_FIELD 1
#define BOX_MENUBUTTON 2
#define BOX_MENU 3

typedef struct {
    int type;
    int data;
    SDL_Rect *rect;
} MouseBox;

typedef struct {
    int width;
    int height;
    int mine_count;
    unsigned char field[MAX_FIELD_SIZE];
} FieldModel;

typedef struct {
    SDL_Surface *screenSurface;
    SDL_Surface *imageSurface;
    FieldModel *fieldModel;
    SDL_Rect fieldRect;
    SDL_Rect startButtonRect;
    SDL_Rect menuButtonRect;
    SDL_Rect *currentFace;
    int need_redraw;
    int width;
    int height;
    int cell_size;
    int color_body;
    int color_light;
    int color_shadow;
} FieldView;

void set_window_rect(int w, int h, int cell_size, SDL_Rect *rect);
void draw_field(FieldView *field);
void draw_panel(SDL_Surface *surface, int x, int y, int width, int height, int frame_width, int body_color, int light_color, int shadow_color, int draw_body);

SDL_Rect red_digits = {0,0,11,21};
SDL_Rect field_digits = {20,21,10,10};

SDL_Rect field_images[] = {
    {0,21,10,10}, {10,21,10,10}, {110,17,15,15},
    {125,17,15,15}, {140,17,15,15},
    {110,0,17,17}, {127,0,17,17},
    {144,0,17,17}, {161,0,17,17},
    {100,21,10,5}, {100,26,10,5}
};
