#include "raylib.h"
#include "raygui.h"   
#include "hud.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800
#define FONT_SIZE 20
#define PROPERTIES_SEP 3.5f
#define COLOR_PANEL    CLITERAL(Color){ 43, 43, 43, 255 }      
#define COLOR_TEXT     CLITERAL(Color){ 187, 187, 187, 255 }  

                                 // Dim   Mass   Ss   sD    Gravity Drag  isBall isSpring isRamp
static HudProperties properties = { 0.0f, 0.0f, 0.0f, 0.0f, false, false, false, false, false};

HudProperties *GetHudProperties(void) {
    return &properties;
}

void RenderHud(void) {

    DrawRectangle(0.75f*SCREEN_WIDTH, 0, SCREEN_WIDTH - (0.75f * SCREEN_WIDTH), GetScreenHeight(), COLOR_PANEL);
    DrawText("Object Type: ",0.75f*SCREEN_WIDTH, 0, FONT_SIZE, COLOR_TEXT);
    GuiToggle((Rectangle){ 0.775f*SCREEN_WIDTH, 25, 120, FONT_SIZE}, TextFormat("Ball"), &properties.ballMenuEnabled);
    GuiToggle((Rectangle){ 0.775f*SCREEN_WIDTH, 25+FONT_SIZE, 120, FONT_SIZE}, TextFormat("Ramp"), &properties.rampMenuEnabled);
    GuiToggle((Rectangle){ 0.775f*SCREEN_WIDTH+120, 25, 120, FONT_SIZE}, TextFormat("Spring"), &properties.springMenuEnabled);

    if (properties.ballMenuEnabled) {
        DrawText("Properties: ",0.75f*SCREEN_WIDTH, 0.25f*SCREEN_HEIGHT, FONT_SIZE, COLOR_TEXT);
        GuiSliderBar((Rectangle){ 0.75f*SCREEN_WIDTH + 3.00f*FONT_SIZE, 0.25f*SCREEN_HEIGHT+FONT_SIZE, 120, FONT_SIZE }, "Radius [px]", TextFormat("%.2f", properties.objectDimension), &properties.objectDimension, 0, 100);
        GuiSliderBar((Rectangle){ 0.75f*SCREEN_WIDTH + 2.85f*FONT_SIZE, 0.25f*SCREEN_HEIGHT+(FONT_SIZE*2)+PROPERTIES_SEP, 120, FONT_SIZE }, "Mass [kg]", TextFormat("%.2f", properties.objectMass), &properties.objectMass, 0, 100);

        GuiCheckBox((Rectangle){ 0.75f*SCREEN_WIDTH + FONT_SIZE, 0.25f*SCREEN_HEIGHT+(FONT_SIZE*3)+PROPERTIES_SEP, 20, 20}, "Gravity Enabled", &properties.isGravity);
        GuiCheckBox((Rectangle){ 0.75f*SCREEN_WIDTH + FONT_SIZE, 0.25f*SCREEN_HEIGHT+(FONT_SIZE*4)+PROPERTIES_SEP, 20, 20}, "Drag Enabled", &properties.isDrag);
    } else if (properties.springMenuEnabled) {
        DrawText("Properties: ",0.75f*SCREEN_WIDTH, 0.25f*SCREEN_HEIGHT, FONT_SIZE, COLOR_TEXT);
        GuiSliderBar((Rectangle){ 0.775f*SCREEN_WIDTH + 2.85f*FONT_SIZE, 0.25f*SCREEN_HEIGHT+(FONT_SIZE*2)+PROPERTIES_SEP, 120, FONT_SIZE }, "Stiffness [N/m]", TextFormat("%.2f", properties.springStiffness), &properties.springStiffness, 0, 100);
        GuiSliderBar((Rectangle){ 0.775f*SCREEN_WIDTH + 2.85f*FONT_SIZE, 0.25f*SCREEN_HEIGHT+(FONT_SIZE*3)+PROPERTIES_SEP, 120, FONT_SIZE }, "Damping [kg]", TextFormat("%.2f", properties.springDamping), &properties.springDamping, 0, 100);
    } else if (properties.rampMenuEnabled) {
        DrawText("Select Two Points on Scene to place Ramp.",0.78f*SCREEN_WIDTH, 0.25f*SCREEN_HEIGHT, FONT_SIZE/2, COLOR_TEXT);;
    }
}
