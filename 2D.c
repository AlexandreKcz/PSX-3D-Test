#include <libgs.h>
#include <stdio.h>
#include "constants.h"
#include <stdlib.h>

#define Sprite GsSPRITE
#define ROT_ONE 4096
#define TYPE_LINE 0
#define TYPE_BOX 1

typedef struct {
    LINE_F2 line;
    int type;
} Line;

typedef  struct {
    Line line[4];
    int type;
} Box;

typedef struct {
    RECT rect;
    RECT crect;
    GsIMAGE tim_data;
    GsSPRITE sprite;
} Image;

void sprite_create(unsigned char imageData[], int x, int y, GsSPRITE **sprite, int bitmode) {
    GsIMAGE* tim_data;
    RECT* rect;
    RECT* crect;
    tim_data = malloc3(sizeof(GsIMAGE));
    GsGetTimInfo((u_long *) (imageData+4), tim_data);
    rect = malloc3(sizeof(RECT));
    crect = malloc3(sizeof(RECT));

    rect->x = tim_data->px;
    rect->y = tim_data->py;
    rect->w = tim_data->pw;
    rect->h = tim_data->ph;

    printf("Framebuffer info {x=%d, y=%d, w=%d, h=%d}\n", rect->x, rect->y, rect->w, rect->h);
    LoadImage(rect, tim_data->pixel);

    crect->x = tim_data->cx;
    crect->y = tim_data->cy;
    crect->w = tim_data->cw;
    crect->h = tim_data->ch;
    
    printf("CLUT info {x=%d, y=%d, w=%d, h=%d}\n", crect->x, crect->y, crect->w, crect->h);
	LoadImage(crect, tim_data->clut);

    *sprite = malloc3(sizeof(GsSPRITE));
    (*sprite) -> attribute = 0x0000000;
    (*sprite) -> x = x;
    (*sprite) -> y = y;
    (*sprite) -> w = tim_data->pw * 4;
    (*sprite) -> h = tim_data->ph;
    (*sprite) -> tpage = getTPage(bitmode, 1, rect->x, rect->y);
    (*sprite) -> r = 128;
    (*sprite) -> g = 128;
    (*sprite) -> b = 128;
    (*sprite) -> u = (tim_data->px * 4) %256;
    (*sprite) -> v = (tim_data->py % 256);
    (*sprite) -> cx = tim_data->cx;
    (*sprite) -> cy = tim_data->cy;
    (*sprite) -> rotate = ROT_ONE * 0;
    (*sprite) -> mx = tim_data->ph/2;
    (*sprite) -> my = tim_data->ph/2;
    (*sprite) -> scalex = ONE * 1;
    (*sprite) -> scaley = ONE * 1;

    free3(rect);
    free3(crect);
    free3(tim_data);
}

void sprite_set_rotation(Sprite* sprite, int rotation){
    sprite -> rotate = ROT_ONE * rotation;
}

void sprite_set_middle(Sprite* sprite, int relative_x, int relative_y) {
    sprite->mx = relative_x;
    sprite->my = relative_y;
}

void sprite_set_position(Sprite* sprite, int x, int y) {
    sprite->x = x;
    sprite->y = y;
}

void sprite_set_blend_color(Sprite* sprite, Color* color){
    sprite->r = color->r;
    sprite->g = color->g;
    sprite->b = color->b;
}

void sprite_set_blend_rgb(Sprite* sprite, int r, int g, int b){
    sprite->r = r;
    sprite->g = g;
    sprite->b = b;
}

void draw_sprite(Sprite* sprite){
    myActiveBuff = GsGetActiveBuff();
    GsSortSprite(sprite, &orderingTable[myActiveBuff], 0);
}

void line_create(Color* color, int x1, int y1, int x2, int y2, Line* line) {
    line = malloc(sizeof(Line));
    line -> type = TYPE_LINE;
    setLineF2(&line->line);
    setRGB0(&line->line, color->r, color->g, color->b);
    setXY2(&line->line, x1, y1, x2, y2);
}

void box_create(Color* color, int x1, int y1, int x2, int y2, Box* box) {
    box = malloc(sizeof(Box));
    line_create(color, x1, y1, x2, y1, &box->line[0]);
    line_create(color, x1, y2, x2, y2, &box->line[1]);
    line_create(color, x1, y1, x1, y2, &box->line[2]);
    line_create(color, x2, y1, x2, y2, &box->line[3]);
}

void line_move(Line* line, int x1, int y1, int x2, int y2){
    line->line.x0 = x1;
    line->line.y0 = y1;
    line->line.x1 = x2;
    line->line.y1 = y2;
}

void box_move(Box* box, int x1, int y1) {
    int* current_width;
    int* current_height;
    int* x2;
    int* y2;
    current_width = malloc(sizeof(int));
    current_height = malloc(sizeof(int));
    x2 = malloc(sizeof(int));
    y2 = malloc(sizeof(int));

    *current_width = box->line[0].line.x1 - box->line[0].line.x0;
    *current_height = box->line[2].line.y1 - box->line[2].line.y1;
    *x2 = x1 + *current_width;
    *y2 = y1 + *current_height;

    line_move(&box->line[0], x1, y1, *x2, y1);
    line_move(&box->line[1], x1, *y2, *x2, *y2);
    line_move(&box->line[2], x1, y1, x1, *y2);
    line_move(&box->line[3], *x2, y1, *x2, *y2);

    free3(current_width);
    free3(current_height);
    free3(x2);
    free3(y2);
}

void draw_line(Line *line){
    DrawPrim(&line->line);
}

void draw_box(Box *box) {
    int i;
    for (int i = 0; i < 4; i++) {
        DrawPrim(&box->line[i].line);
    }
}