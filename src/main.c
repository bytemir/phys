#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"   

#include <stdio.h>

// Engine Includes
#include "hud.h"


#define MAX_OBJECTS 100

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800
#define COLOR_BG CLITERAL(Color){ 47, 50, 51, 255 }      
#define OBJECT_COLOR CLITERAL(Color){ 255, 255, 255, 255 }      

typedef enum {
    BALL
} ObjectType;

typedef struct {
    
    Vector2 PositionMap[MAX_OBJECTS];
    Vector2 VelocityMap[MAX_OBJECTS];
    Vector2 ForceMap[MAX_OBJECTS];
    float MassMap[MAX_OBJECTS];
    float DimensionMap[MAX_OBJECTS];
    ObjectType ObjectMap[MAX_OBJECTS];

    int num_objects;

} Scene;

static Scene engineScene;

void RenderScene(void) {
    for (int object_index = 0; object_index < engineScene.num_objects; object_index++) {
        if (engineScene.ObjectMap[object_index] == BALL) {
            DrawCircleLinesV(engineScene.PositionMap[object_index],engineScene.DimensionMap[object_index],OBJECT_COLOR);
        }
    }
}

int main(void)
{
    

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Physics Engine");
    SetTargetFPS(60);

    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xBBBBBBFF);
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x3C3F41FF);
    GuiSetStyle(SLIDER, BORDER_COLOR_NORMAL, 0x499CE2FF);

    while (!WindowShouldClose())
    {

        HudProperties *hudData = GetHudProperties();
        Vector2 MousePosition = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && MousePosition.x < 0.75f * SCREEN_WIDTH && hudData->objectDimension != 0) {
            if (hudData->ballMenuEnabled) {
                printf("Spawning Ball at X:%.1f Y:%.1f Radius: %.2f (px) Mass: %.2f (kg) (Total: %d)\n", MousePosition.x, MousePosition.y,hudData->objectDimension,hudData->objectMass, engineScene.num_objects + 1);
                engineScene.PositionMap[engineScene.num_objects] = MousePosition;
                engineScene.VelocityMap[engineScene.num_objects] = (Vector2){ 0.0f, 0.0f };
                engineScene.ForceMap[engineScene.num_objects] = (Vector2){ 0.0f, -9.81f };
                engineScene.MassMap[engineScene.num_objects] = hudData->objectMass;
                engineScene.DimensionMap[engineScene.num_objects] = hudData->objectDimension;
                engineScene.ObjectMap[engineScene.num_objects] = (ObjectType){ BALL };
                engineScene.num_objects++;
            }
        }

        BeginDrawing();
        ClearBackground(COLOR_BG);
        

        RenderHud();
        RenderScene();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}