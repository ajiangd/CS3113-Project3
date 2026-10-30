/**
* Author: Ashley Jiang
* Assignment: Lunar Lander
* Date due: 10/30/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;

constexpr char    BG_COLOUR[] = "#87CEEB";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float ACCELERATION_OF_GRAVITY = 40.0f,
                BURNER_ACCELERATION = 100.0f,
                VENT_ACCELERATION = 80.0f,
                AIR_DRAG = 1.5f;

//Balloon variables
constexpr char    BALLOON_FP[]     = "assets/game/balloon.png";
constexpr Vector2 BALLOON_SIZE     = { 60.0f, 77.0f },
                  BALLOON_INIT_POS = { ORIGIN.x, 80.0f };

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gPreviousTicks = 0.0f;

Entity *gBalloon = nullptr;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hot Air Balloon Lander");

    /*
        ----------- BALLOON -----------
    */
    gBalloon = new Entity(BALLOON_INIT_POS, BALLOON_SIZE, BALLOON_FP);
    gBalloon->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    SetTargetFPS(FPS);
}

void processInput()
{
    Vector2 acceleration = { 0.0f, ACCELERATION_OF_GRAVITY };

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE))
        acceleration.y -= BURNER_ACCELERATION;

    if      (IsKeyDown(KEY_A)) acceleration.x = -VENT_ACCELERATION;
    else if (IsKeyDown(KEY_D)) acceleration.x =  VENT_ACCELERATION;
    else
        acceleration.x = -gBalloon->getVelocity().x * AIR_DRAG;

    gBalloon->setAcceleration(acceleration);
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    gBalloon->update(deltaTime, nullptr, 0);
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    gBalloon->render();

    EndDrawing();
}

void shutdown()
{
    delete gBalloon;
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}