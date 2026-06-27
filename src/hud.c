#include "raylib.h"
#include "raygui.h"   
#include "hud.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800
#define FONT_SIZE 20
#define PROPERTIES_SEP 3.5f
#define COLOR_PANEL    CLITERAL(Color){ 43, 43, 43, 255 }      
#define COLOR_TEXT     CLITERAL(Color){ 187, 187, 187, 255 }  
#define COLOR_TEXT_GREEN     CLITERAL(Color){ 0, 255, 0, 255 }  

                                 // Dim  Mass  isBall  Gravity Drag
static HudProperties properties = { 0.0f, 0.0f, false, false, false};

HudProperties *GetHudProperties(void) {
    return &properties;
}

void RenderHud(void) {

    DrawText(TextFormat("FPS: %i", GetFPS()),0, 0, FONT_SIZE, COLOR_TEXT);

    DrawRectangle(0.75f*SCREEN_WIDTH, 0, SCREEN_WIDTH - (0.75f * SCREEN_WIDTH), GetScreenHeight(), COLOR_PANEL);
    DrawText("Object Type: ",0.75f*SCREEN_WIDTH, 0, FONT_SIZE, COLOR_TEXT);
    GuiToggle((Rectangle){ 0.75f*SCREEN_WIDTH, 25, 120, FONT_SIZE}, TextFormat("Ball"), &properties.ballMenuEnabled);

    if (properties.ballMenuEnabled) {
        DrawText("Properties: ",0.75f*SCREEN_WIDTH, 0.25f*SCREEN_HEIGHT, FONT_SIZE, COLOR_TEXT);
        GuiSliderBar((Rectangle){ 0.75f*SCREEN_WIDTH + 3.00f*FONT_SIZE, 0.25f*SCREEN_HEIGHT+FONT_SIZE, 120, FONT_SIZE }, "Radius [px]", TextFormat("%.2f", properties.objectDimension), &properties.objectDimension, 0, 100);
        GuiSliderBar((Rectangle){ 0.75f*SCREEN_WIDTH + 2.85f*FONT_SIZE, 0.25f*SCREEN_HEIGHT+(FONT_SIZE*2)+PROPERTIES_SEP, 120, FONT_SIZE }, "Mass [kg]", TextFormat("%.2f", properties.objectMass), &properties.objectMass, 0, 100);

        GuiCheckBox((Rectangle){ 0.75f*SCREEN_WIDTH + FONT_SIZE, 0.25f*SCREEN_HEIGHT+(FONT_SIZE*3)+PROPERTIES_SEP, 20, 20}, "Gravity Enabled", &properties.isGravity);
        GuiCheckBox((Rectangle){ 0.75f*SCREEN_WIDTH + FONT_SIZE, 0.25f*SCREEN_HEIGHT+(FONT_SIZE*4)+PROPERTIES_SEP, 20, 20}, "Drag Enabled", &properties.isDrag);
    }
}
