#include <libgs.h>
#include "constants.h"

#define MAX_OBJECTS 100

GsDOBJ2 Object[MAX_OBJECTS];
int ObjectCount = 0;

GsF_LIGHT sunLight[1];
VECTOR sunDirection;
Color sunColor;
Color ambientLightColor;

void SetSunColor (int r, int g, int b) {
    sunColor = createColor(r, g, b);
}

void SetSunDirection (int x, int y, int z) {
    sunDirection.vx = x;
    sunDirection.vy = y;
    sunDirection.vz = z;
}

void SetAmbientLight (int r, int g, int b) {
    ambientLightColor = createColor(r, g, b);

    GsSetAmbient(
        ambientLightColor.r * (ONE/255),
        ambientLightColor.g * (ONE/255),
        ambientLightColor.b * (ONE/255)
    );
}

void CalculateCamera() {
    VECTOR vec;
    GsVIEW2 view;

    view.view = Camera.coord2.coord;
    view.super = WORLD;

    RotMatrix(&Camera.rotation, &view.view);

    ApplyMatrixLV(&view.view, &Camera.position, &vec);

    TransMatrix(&view.view, &vec);

    GsSetView2(&view);

    sunLight[0].vx = sunDirection.vx;
    sunLight[0].vy = -sunDirection.vy;
    sunLight[0].vz = sunDirection.vz;

    sunLight[0].r = sunColor.r;
    sunLight[0].g = sunColor.g;
    sunLight[0].b = sunColor.b;
    GsSetFlatLight(0, &sunLight[0]);
}

void RenderObject(VECTOR pos, SVECTOR rot, GsDOBJ2 *obj) {
    MATRIX lmtx, omtx;
    GsCOORDINATE2 coord;

    coord = Camera.coord2;

    pos.vy *= -1;

    RotMatrix(&rot, &omtx);
    TransMatrix(&omtx, &pos);
    CompMatrixLV(&Camera.coord2.coord, &omtx, &coord.coord);
    coord.flg = 0;

    obj->coord2 = &coord;

    GsGetLws(obj->coord2, &lmtx, &omtx);
    GsSetLightMatrix(&lmtx);
    GsSetLsMatrix(&omtx);

    GsSortObject4(obj, &orderingTable[myActiveBuff], 14-OT_LENGTH, getScratchAddr(0));
}

int LoadTMD(u_long *tmd, GsDOBJ2 *obj, int enableLightning) {

    printf("Loading TMD");

    u_long *dop;
    int i, NumObj;

    dop = tmd;
    
    dop++;
    GsMapModelingData(dop);

    dop++;
    NumObj = *dop;

    dop++;
    for (i=0; i<NumObj; i++) {
        GsLinkObject4((u_long)dop, &obj[i], i);

        if (enableLightning == 0) {
            obj[i].attribute = (1<<6);
        }
    }

    printf("TMD Load Done");

    return (NumObj);
}

void loadTexture(unsigned char imageData[]) {
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

    free3(rect);
    free3(crect);
    free3(tim_data);

    printf("Texture loading done");
}

VECTOR pos;
MATRIX tempMatrixPos;
VECTOR returnPos;
VECTOR Translate (SVECTOR rot, int x, int y, int z) {
    pos.vx = x;
    pos.vy = y;
    pos.vz = z;

    RotMatrix(&rot, &tempMatrixPos);

    ApplyMatrixLV(&tempMatrixPos, &pos, &returnPos);

    return returnPos;
}