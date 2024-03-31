#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libetc.h>
#include <libspu.h>
#include <libds.h>
#include <string.h>
#include <sys/types.h>

void clear_vram();

//Screen res and Dither
int SCREEN_WIDTH, SCREEN_HEIGHT;
#define CENTERX SCREEN_WIDTH/2
#define CENTERY SCREEN_HEIGHT/2
#define DITHER 1

#define OT_LENGTH 12
#define OT_ENTRIES 1<<OT_LENGTH
#define PACKETMAX 2048

typedef struct {
    int r;
    int g;
    int b;
} Color;

struct {
    VECTOR position;
    SVECTOR rotation;
    GsCOORDINATE2 coord2;
} Camera;

GsOT    orderingTable[2];
GsOT    orderingTable_TAG[2][OT_ENTRIES];
int     myActiveBuff = 0;
PACKET  GPUOutPacket[2][PACKETMAX*24];
Color   BGColor;

Color createColor(int r, int g, int b) {
    Color color = {.r = r, .g = g, .b = b};
    return color;
}

void setBGColor (int r, int g, int b) {
    BGColor = createColor(r, g, b);
}

void initializeScreen() {
    ResetGraph(0);

    if (*(char *)0xbfc7ff52 == 'E'){
        //PAL Mode
        SCREEN_WIDTH = 320;
    	SCREEN_HEIGHT = 256;
    	printf("Setting the PlayStation Video Mode to (PAL %dx%d)\n",SCREEN_WIDTH,SCREEN_HEIGHT);
    	SetVideoMode(1);
    	printf("Video Mode is (%ld)\n",GetVideoMode());
    } else {
        // NTSC Mode
     	SCREEN_WIDTH = 320;
     	SCREEN_HEIGHT = 240;
     	printf("Setting the PlayStation Video Mode to (NTSC %dx%d)\n",SCREEN_WIDTH,SCREEN_HEIGHT);
     	SetVideoMode(0);
     	printf("Video Mode is (%ld)\n",GetVideoMode());
    }

    GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 0);
    GsDefDispBuff(0, 0, 0, SCREEN_HEIGHT);

    orderingTable[0].length = OT_LENGTH;
    orderingTable[1].length = OT_LENGTH;
    orderingTable[0].org = orderingTable_TAG[0];
    orderingTable[1].org = orderingTable_TAG[1];

    GsClearOt(0, 0, &orderingTable[0]);
    GsClearOt(0, 0, &orderingTable[1]);

    FntLoad(960, 0);
    FntOpen(-CENTERX + 7, -CENTERY + 15, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 512);

    GsInit3D();
    GsSetProjection(CENTERX);
    GsInitCoordinate2(WORLD, &Camera.coord2);

    //0 : no fog || 1 : no fog
    GsSetLightMode(0);
}

void clear_vram() {
    RECT rectTL;
    setRECT(&rectTL, 0, 0, 1024, 512);
    ClearImage2(&rectTL, 0, 0, 0);
    DrawSync(0);
    return;  
}

void clear_display() {
    myActiveBuff = GsGetActiveBuff();
    GsSetWorkBase((PACKET*) GPUOutPacket[myActiveBuff]);
    GsClearOt(0, 0, &orderingTable[myActiveBuff]);
}

void Display() {

    FntFlush(-1);
    VSync(0);
    GsSwapDispBuff();

    GsSortClear(BGColor.r, BGColor.g, BGColor.b, &orderingTable[myActiveBuff]);
    GsDrawOt(&orderingTable[myActiveBuff]);
}