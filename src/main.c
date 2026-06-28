#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"   

#include <stdio.h>

// Engine Includes
#include "hud.h"


// EngineSettings
#define MAX_OBJECTS 100
#define MAX_SPRINGS 100

static float PPM = 40.0f; // Pixels Per Meter
static float gravityConstant = 9.81f;

#define FONT_SIZE 20
#define COLOR_TEXT     CLITERAL(Color){ 187, 187, 187, 255 }  
#define ANCHOR_SIZE 5.0f

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800
#define COLOR_BG CLITERAL(Color){ 47, 50, 51, 255 }      
#define OBJECT_COLOR CLITERAL(Color){ 255, 255, 255, 255 }  

#define ANCHOR_COLOR CLITERAL(Color){ 255, 255, 153, 255 }      

typedef enum {
    BALL,
} ObjectType;


typedef struct {
    Vector2 PositionA;
    Vector2 PositionB;
    float Stiffness;
    float Damping;
} Spring;


typedef struct {
    Vector2 PositionA;
    Vector2 PositionB;
} Ramp;



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

    Spring SpringMap[MAX_OBJECTS];
    int num_springs;

    Ramp RampMap[MAX_OBJECTS];
    int num_ramps;

} Scene;

static Scene engineScene;
static int springCreationIndex = 0;
static int rampCreationIndex = 0;

void RenderScene(void) {

    if (rampCreationIndex == 1) {
        DrawCircleV(engineScene.RampMap[engineScene.num_ramps].PositionA,3.0f,OBJECT_COLOR);
        DrawLineEx(engineScene.RampMap[engineScene.num_ramps].PositionA, GetMousePosition(),3.0f, OBJECT_COLOR);
    }

    for (int ramp_index = 0; ramp_index < engineScene.num_ramps; ramp_index++) {
        DrawLineEx(engineScene.RampMap[ramp_index].PositionA, engineScene.RampMap[ramp_index].PositionB,3.0f, OBJECT_COLOR);
        DrawCircleV(engineScene.RampMap[ramp_index].PositionA, 3.0f, OBJECT_COLOR);
        DrawCircleV(engineScene.RampMap[ramp_index].PositionB, 3.0f, OBJECT_COLOR);
    }  
    
    if (springCreationIndex == 1) {
        DrawCircleV(engineScene.SpringMap[engineScene.num_springs].PositionA,ANCHOR_SIZE,ANCHOR_COLOR);
        DrawLineEx(engineScene.SpringMap[engineScene.num_springs].PositionA, GetMousePosition(),3.0f, OBJECT_COLOR);
    }

    for (int spring_index = 0; spring_index < engineScene.num_springs; spring_index++) {
        DrawLineEx(engineScene.SpringMap[spring_index].PositionA, engineScene.SpringMap[spring_index].PositionB,3.0f, OBJECT_COLOR);
        DrawCircleV(engineScene.SpringMap[spring_index].PositionA, ANCHOR_SIZE, ANCHOR_COLOR);
        DrawCircleV(engineScene.SpringMap[spring_index].PositionB, ANCHOR_SIZE, ANCHOR_COLOR);
    }  

    for (int object_index = 0; object_index < engineScene.num_objects; object_index++) {
        if (engineScene.ObjectMap[object_index] == BALL) {
            DrawCircleLinesV(engineScene.PositionMap[object_index],engineScene.DimensionMap[object_index],OBJECT_COLOR);
        }
    }
}

void ResolveCollisions(void) {
    for (int i = 0; i < engineScene.num_objects; i++) {
        for (int j = i + 1; j < engineScene.num_objects; j++) {
            
            if (CheckCollisionCircles(engineScene.PositionMap[i], engineScene.DimensionMap[i], engineScene.PositionMap[j], engineScene.DimensionMap[j])) {
                
                float deltaX = engineScene.PositionMap[j].x - engineScene.PositionMap[i].x;
                float deltaY = engineScene.PositionMap[j].y - engineScene.PositionMap[i].y;
                float distance = sqrtf(deltaX * deltaX + deltaY * deltaY);
                
                if (distance == 0.0f) continue;

                float nx = deltaX / distance;
                float ny = deltaY / distance;

                float minimumDistance = engineScene.DimensionMap[i] + engineScene.DimensionMap[j];
                float depth = minimumDistance - distance;
                
                engineScene.PositionMap[i].x -= nx * (depth * 0.5f);
                engineScene.PositionMap[i].y -= ny * (depth * 0.5f);
                engineScene.PositionMap[j].x += nx * (depth * 0.5f);
                engineScene.PositionMap[j].y += ny * (depth * 0.5f);

                float deltaVelocityX = engineScene.VelocityMap[i].x - engineScene.VelocityMap[j].x;
                float deltaVelocityY = engineScene.VelocityMap[i].y - engineScene.VelocityMap[j].y;
                float normalVelocity = deltaVelocityX * nx + deltaVelocityY * ny;

                if (normalVelocity > 0.0f) {
                    float totalMass = engineScene.MassMap[i] + engineScene.MassMap[j];
                    if (totalMass <= 0.0f) continue;

                    float restitution = 0.8f; 
                    float magnitudeImpulseForce = (1.0f + restitution) * normalVelocity / totalMass;

                    engineScene.VelocityMap[i].x -= magnitudeImpulseForce * engineScene.MassMap[j] * nx;
                    engineScene.VelocityMap[i].y -= magnitudeImpulseForce * engineScene.MassMap[j] * ny;
                    engineScene.VelocityMap[j].x += magnitudeImpulseForce * engineScene.MassMap[i] * nx;
                    engineScene.VelocityMap[j].y += magnitudeImpulseForce * engineScene.MassMap[i] * ny;
                }
            }
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
        
        // Apply Gravity Force
        if (engineScene.GravityMap[i]) {  engineScene.ForceMap[i].y += engineScene.MassMap[i] * gravityConstant; }


        // Apply Drag Force
        if (engineScene.DragMap[i]) {
            float dragCoefficient = 5.0f;
            engineScene.ForceMap[i].x -= dragCoefficient * engineScene.VelocityMap[i].x;
            engineScene.ForceMap[i].y -= dragCoefficient * engineScene.VelocityMap[i].y;
        }

        // Apply Spring Force
        Vector2 ballPosition = engineScene.PositionMap[i];
        float ballRadius = engineScene.DimensionMap[i];

        for (int s = 0; s < engineScene.num_springs; s++) {
            Vector2 position1 = engineScene.SpringMap[s].PositionA;
            Vector2 position2 = engineScene.SpringMap[s].PositionB;

            float deltaX = position2.x - position1.x;
            float deltaY = position2.y - position1.y;
            float lengthSquared = deltaX * deltaX + deltaY * deltaY;
            
            if (lengthSquared == 0.0f) continue; 

            float projection = ((ballPosition.x - position1.x) * deltaX + (ballPosition.y - position1.y) * deltaY) / lengthSquared;
            
            if (projection < 0.0f) projection = 0.0f;
            if (projection > 1.0f) projection = 1.0f;

            Vector2 closestPoint = { position1.x + projection * deltaX, position1.y + projection * deltaY };

            float dx = ballPosition.x - closestPoint.x;
            float dy = ballPosition.y - closestPoint.y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance < ballRadius && distance > 0.0f) {
                float nx = dx / distance;
                float ny = dy / distance;

                float depth = ballRadius - distance;
                float magnitudeSpringForce = depth * engineScene.SpringMap[s].Stiffness * PPM;

                float normalVelocity = engineScene.VelocityMap[i].x * nx + engineScene.VelocityMap[i].y * ny;
                float magnitudeDampingForce = normalVelocity * engineScene.SpringMap[s].Damping;

                float magnitudePushForce = magnitudeSpringForce - magnitudeDampingForce;
                if (magnitudePushForce < 0.0f) magnitudePushForce = 0.0f; 

                engineScene.ForceMap[i].x += nx * magnitudePushForce;
                engineScene.ForceMap[i].y += ny * magnitudePushForce;
            }
        }
        for (int r = 0; r < engineScene.num_ramps; r++) {
            Vector2 position1 = engineScene.RampMap[r].PositionA;
            Vector2 position2 = engineScene.RampMap[r].PositionB;

            float deltaX = position2.x - position1.x;
            float deltaY = position2.y - position1.y;
            float lengthSquared = deltaX * deltaX + deltaY * deltaY;

            if (lengthSquared == 0.0f) continue;

            float projection = ((ballPosition.x - position1.x) * deltaX + (ballPosition.y - position1.y) * deltaY) / lengthSquared;

            if (projection < 0.0f) projection = 0.0f;
            if (projection > 1.0f) projection = 1.0f;

            Vector2 closestPoint = { position1.x + projection * deltaX, position1.y + projection * deltaY };

            float dx = ballPosition.x - closestPoint.x;
            float dy = ballPosition.y - closestPoint.y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance < ballRadius && distance > 0.0f) {
                float nx = dx / distance;
                float ny = dy / distance;

                float depth = ballRadius - distance;

                engineScene.PositionMap[i].x += nx * depth;
                engineScene.PositionMap[i].y += ny * depth;
                ballPosition = engineScene.PositionMap[i];

                float normalVelocity = engineScene.VelocityMap[i].x * nx + engineScene.VelocityMap[i].y * ny;

                if (normalVelocity < 0.0f) {
                    float restitution = 0.7f;
                    engineScene.VelocityMap[i].x -= (1.0f + restitution) * normalVelocity * nx;
                    engineScene.VelocityMap[i].y -= (1.0f + restitution) * normalVelocity * ny;
                }
            }
        }

        if (engineScene.MassMap[i] > 0.0f) {  // Update Velocity
            Vector2 acceleration = {
                engineScene.ForceMap[i].x / engineScene.MassMap[i],
                engineScene.ForceMap[i].y / engineScene.MassMap[i]
            };

            engineScene.VelocityMap[i].x += acceleration.x * deltaTime;
            engineScene.VelocityMap[i].y += acceleration.y * deltaTime;
        }
        

        // Update Position
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

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && MousePosition.x < 0.75f * SCREEN_WIDTH) {

            if (hudData->ballMenuEnabled && hudData->objectDimension != 0 && engineScene.num_objects < MAX_OBJECTS) {
                printf("Spawning Ball at X:%.1f Y:%.1f Radius: %.2f (px) Mass: %.2f (kg) (Total: %d)\n", MousePosition.x, MousePosition.y,hudData->objectDimension,hudData->objectMass, engineScene.num_objects + 1);
                printf("With Applied Forces -> Gravity: %s, Drag: %s\n", hudData->isGravity?"true":"false", hudData->isDrag?"true":"false");
                Vector2 mouseSpeed = GetMouseDelta();
                
                float throwMultiplier = 1.5f; 

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
            else if (hudData->springMenuEnabled) {
                if (springCreationIndex == 0) {
                    springCreationIndex = 1;
                    printf("Set Spring PosA: (%.1f,%.1f)\n", MousePosition.x, MousePosition.y);
                    engineScene.SpringMap[engineScene.num_springs] = (Spring) {MousePosition,MousePosition, hudData->springStiffness, hudData->springDamping};
                }
                else if (springCreationIndex == 1) {
                    springCreationIndex = 0;
                    printf("Set Spring PosB: (%.1f,%.1f)\n", MousePosition.x, MousePosition.y);
                    engineScene.SpringMap[engineScene.num_springs].PositionB = MousePosition;
                    printf("Spawned Spring at A: (%.1f,%.1f) -> B: (%.1f,%.1f)\n", engineScene.SpringMap[engineScene.num_springs].PositionA.x, engineScene.SpringMap[engineScene.num_springs].PositionA.y,MousePosition.x, MousePosition.y);
                    printf("Stiffness: %.2f, Damping: %.2f (Total: %i)\n", engineScene.SpringMap[engineScene.num_springs].Stiffness, engineScene.SpringMap[engineScene.num_springs].Damping, engineScene.num_springs);
                    engineScene.num_springs++;
                }

            }
            else if (hudData->rampMenuEnabled) {
                if (rampCreationIndex == 0) {
                    rampCreationIndex = 1;
                    printf("Set Ramp PosA: (%.1f,%.1f)\n", MousePosition.x, MousePosition.y);
                    engineScene.RampMap[engineScene.num_ramps] = (Ramp) {MousePosition,MousePosition};
                }
                else if (rampCreationIndex == 1) {
                    rampCreationIndex = 0;
                    printf("Set Ramp PosB: (%.1f,%.1f)\n", MousePosition.x, MousePosition.y);
                    engineScene.RampMap[engineScene.num_ramps].PositionB = MousePosition;
                    printf("Spawned Ramp at A: (%.1f,%.1f) -> B: (%.1f,%.1f)\n", engineScene.RampMap[engineScene.num_ramps].PositionA.x, engineScene.RampMap[engineScene.num_ramps].PositionA.y,MousePosition.x, MousePosition.y);
                    printf("(Total: %i)", engineScene.num_ramps);
                    engineScene.num_ramps++;
                }

            }
        }

        UpdateScene(deltaTime);
        ResolveCollisions();
        
        BeginDrawing();

        BeginBlendMode(BLEND_ALPHA);
        ClearBackground(COLOR_BG);

        RenderHud();
        DrawText(TextFormat("FPS: %i", GetFPS()),0, 0, FONT_SIZE, COLOR_TEXT);
        DrawText(TextFormat("Objects: %i", engineScene.num_objects),0, FONT_SIZE, FONT_SIZE, COLOR_TEXT);
        DrawText(TextFormat("Springs: %i", engineScene.num_springs),0, FONT_SIZE*2, FONT_SIZE, COLOR_TEXT);
        DrawText(TextFormat("Ramps: %i", engineScene.num_ramps),0, FONT_SIZE*3, FONT_SIZE, COLOR_TEXT);
        RenderScene();

        EndBlendMode();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}