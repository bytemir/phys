#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"   

#include <stdio.h>

// Engine Includes
#include "hud.h"


// EngineSettings
#define MAX_OBJECTS 100
static float PPM = 40.0f; // Pixels Per Meter
static float gravityConstant = 9.81f;

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

    bool GravityMap[MAX_OBJECTS];
    bool DragMap[MAX_OBJECTS];

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

void ApplyBoundaryConditions(int i) {
    if (engineScene.PositionMap[i].x - engineScene.DimensionMap[i] <= 0) {
        engineScene.PositionMap[i].x = engineScene.DimensionMap[i];
        engineScene.VelocityMap[i].x *= -1;
    } 
    else if (engineScene.PositionMap[i].x + engineScene.DimensionMap[i] > 0.75f * SCREEN_WIDTH) {
        engineScene.PositionMap[i].x = 0.75f * SCREEN_WIDTH - engineScene.DimensionMap[i];
        engineScene.VelocityMap[i].x *= -1;
    }

    if (engineScene.PositionMap[i].y - engineScene.DimensionMap[i] <= 0) {
        engineScene.PositionMap[i].y = engineScene.DimensionMap[i];
        engineScene.VelocityMap[i].y *= -1;
    } 
    else if (engineScene.PositionMap[i].y + engineScene.DimensionMap[i] > SCREEN_HEIGHT) {
        engineScene.PositionMap[i].y = SCREEN_HEIGHT - engineScene.DimensionMap[i];
        engineScene.VelocityMap[i].y *= -1;
    }
}

void UpdateScene(float deltaTime) {
    for (int i = 0; i < engineScene.num_objects; i++) {
        engineScene.ForceMap[i] = (Vector2){ 0.0f, 0.0f };

        if (engineScene.GravityMap[i]) { // F = mg
            Vector2 gravityForce = { 0.0f, engineScene.MassMap[i] * gravityConstant };
            
            engineScene.ForceMap[i].x += gravityForce.x;
            engineScene.ForceMap[i].y += gravityForce.y;
        }

        if (engineScene.DragMap[i]) {
            float dragCoefficient = 5.0f;

            Vector2 dragForce = {
                -dragCoefficient * engineScene.VelocityMap[i].x,
                -dragCoefficient * engineScene.VelocityMap[i].y
            };

            engineScene.ForceMap[i].x += dragForce.x;
            engineScene.ForceMap[i].y += dragForce.y;
        }


        if (engineScene.MassMap[i] > 0.0f) { // Update Resultant Force
            Vector2 acceleration = {
                engineScene.ForceMap[i].x / engineScene.MassMap[i],
                engineScene.ForceMap[i].y / engineScene.MassMap[i]
            };

            engineScene.VelocityMap[i].x += acceleration.x * deltaTime;
            engineScene.VelocityMap[i].y += acceleration.y * deltaTime;
        }

        // Updating Positions
        engineScene.PositionMap[i].x += engineScene.VelocityMap[i].x * PPM * deltaTime;
        engineScene.PositionMap[i].y += engineScene.VelocityMap[i].y * PPM * deltaTime;

        ApplyBoundaryConditions(i);
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

        float deltaTime = GetFrameTime();

        HudProperties *hudData = GetHudProperties();
        Vector2 MousePosition = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && MousePosition.x < 0.75f * SCREEN_WIDTH && hudData->objectDimension != 0 && engineScene.num_objects < MAX_OBJECTS) {

            if (hudData->ballMenuEnabled) {
                printf("Spawning Ball at X:%.1f Y:%.1f Radius: %.2f (px) Mass: %.2f (kg) (Total: %d)\n", MousePosition.x, MousePosition.y,hudData->objectDimension,hudData->objectMass, engineScene.num_objects + 1);
                printf("With Applied Forces -> Gravity: %s, Drag: %s\n", hudData->isGravity?"true":"false", hudData->isDrag?"true":"false");
                Vector2 mouseSpeed = GetMouseDelta();
                
                float throwMultiplier = 1.0f; 

                engineScene.PositionMap[engineScene.num_objects] = MousePosition;
                
                engineScene.VelocityMap[engineScene.num_objects] = (Vector2){
                    mouseSpeed.x * throwMultiplier,
                    mouseSpeed.y * throwMultiplier
                };

                engineScene.ForceMap[engineScene.num_objects] = (Vector2){ 0.0f, 0.0f };
                engineScene.MassMap[engineScene.num_objects] = hudData->objectMass;

                engineScene.GravityMap[engineScene.num_objects] = hudData->isGravity;
                engineScene.DragMap[engineScene.num_objects] = hudData->isDrag;

                engineScene.DimensionMap[engineScene.num_objects] = hudData->objectDimension;
                engineScene.ObjectMap[engineScene.num_objects] = (ObjectType){ BALL };
                engineScene.num_objects++;
            }
        }


        
        UpdateScene(deltaTime);
        BeginDrawing();
        ClearBackground(COLOR_BG);
        

        RenderHud();
        RenderScene();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}