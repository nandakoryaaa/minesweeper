#define MAX_FIELD_SIZE 10000
#define F_QUESTION     0b10000000
#define F_FLAG         0b01000000
#define F_CLOSED       0b00100000
#define F_MINE         0b00010000
#define F_NUMBER       0b00001111

#define COLOR_BODY   0xc0c0c0
#define COLOR_LIGHT  0xffffff
#define COLOR_SHADOW 0x808080

typedef enum {
    GAME_INIT, GAME_PLAY, GAME_STOP, GAME_MENU
} EnumGameMode;

typedef enum {
    ICON_QUESTION, ICON_FLAG, ICON_MINE, ICON_RED_MINE, ICON_ERROR,
    SMILEY_HAPPY, SMILEY_SCARED, SMILEY_DEAD, SMILEY_COOL,
    MENU_OPEN, MENU_CLOSE
} EnumImageId;

typedef enum {
    BOX_STARTBUTTON, BOX_MENUBUTTON, BOX_FIELD, BOX_MENUITEM
} EnumMouseBoxType;

typedef enum {
    MENUITEM_BEGINNER, MENUITEM_INTERMEDIATE, MENUITEM_EXPERT
} EnumMenuitemId;

typedef struct {
    EnumMouseBoxType type;
    int data;
    SDL_Rect *rect;
} MouseBox;

typedef struct {
    int width;
    int height;
    int total;
    int mine_count;
    unsigned char field[MAX_FIELD_SIZE];
} FieldModel;

typedef struct {
    SDL_Window *window;
    SDL_Surface *screenSurface;
    SDL_Surface *imageSurface;
    FieldModel *fieldModel;
    SDL_Rect fieldRect;
    SDL_Rect startButtonRect;
    SDL_Rect menuButtonRect;
    SDL_Rect menuItemBeginnerRect;
    SDL_Rect menuItemIntermediateRect;
    SDL_Rect menuItemExpertRect;
    SDL_Rect *currentFace;
    MouseBox menuItemBeginnerBox;
    MouseBox menuItemIntermediateBox;
    MouseBox menuItemExpertBox;
    MouseBox *box_list[10];
    int timer;
    int start_ticks;
    int open_cells;
    int box_list_size;
    EnumGameMode game_mode;
    EnumGameMode old_game_mode;
    int explosion_addr;
    int width;
    int height;
    int cell_size;
    int color_body;
    int color_light;
    int color_shadow;
    int color_explosion;
    int need_redraw;
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
