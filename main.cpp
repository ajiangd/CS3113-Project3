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

constexpr float ACCELERATION_OF_GRAVITY = 20.0f,
                BURNER_ACCELERATION = 100.0f,
                VENT_ACCELERATION = 80.0f,
                AIR_DRAG = 1.5f;

//Balloon variables
constexpr char    BALLOON_FP[]     = "assets/game/balloon.png";
constexpr Vector2 BALLOON_SIZE     = { 60.0f, 77.0f },
                  BALLOON_INIT_POS = { ORIGIN.x, 80.0f };

//Fuel variables
constexpr char FUEL_DIGITS_FP[] = "assets/game/digits.png",
            FUEL_LABEL_FP[]   = "assets/game/fuel_label.png",
            FUEL_LOW_COLOUR[] = "#FF5252";
constexpr int FUEL_DIGIT_COUNT = 4,
            DIGITS_COLS = 10;
constexpr float MAX_FUEL = 1000.0f,
            BURNER_FUEL_RATE = 120.0f,
            VENT_FUEL_RATE  = 60.0f,
            LOW_FUEL = 250.0f,
            FUEL_LABEL_GAP = 12.0f;
constexpr Vector2 FUEL_IND_POS = { 20.0f, 20.0f },
            FUEL_LABEL_SIZE = { 84.0f, 30.0f },
            DIGIT_SIZE = { 21.0f, 30.0f };

// Sky variables
constexpr char SKY_FP[] = "assets/game/sky.png";
constexpr Vector2 SKY_SIZE = {SCREEN_WIDTH, SCREEN_HEIGHT};

// Water varaibles
constexpr char WATER_FP[] = "assets/game/water.png";
constexpr Vector2 WATER_SIZE = {SCREEN_WIDTH, 140.0f},
                    WATER_POS = {ORIGIN.x, SCREEN_HEIGHT-WATER_SIZE.y / 2.0f};
constexpr float   WATER_LINE = SCREEN_HEIGHT - WATER_SIZE.y;

// Island variables
constexpr char ISLAND_FP[] = "assets/game/island.png";
constexpr Vector2 ISLAND_SIZE = { 200.0f, 128.0f},
                    ISLAND_POS      = { 820.0f, WATER_LINE },
                    ISLAND_COLLIDER = {120.0f, 10.0f};

// Raft
constexpr char RAFT_FP[] = "assets/game/raft.png";
constexpr Vector2 RAFT_SIZE = { 120.0f, 53.0f },
                    RAFT_COLLIDER = { 100.0f, 20.0f };
constexpr float   RAFT_Y = WATER_LINE - 5.0f,
                    RAFT_CENTER_X = 380.0f,
                    RAFT_RANGE = 200.0f,
                    RAFT_SPEED = 0.6f;

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gPreviousTicks = 0.0f;
float gFuel = MAX_FUEL;
bool gIsBurning = false,
        gIsVenting = false;
Texture2D gDigitsTexture;
Texture2D gFuelLabelTexture;

Entity *gBalloon = nullptr;
Entity *gSky = nullptr;
Entity *gWater = nullptr;
Entity *gIsland = nullptr;
Entity *gRaft = nullptr;

float gRaftTime = 0.0f;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
void renderFuel();
void restrictBalloon();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hot Air Balloon Lander");

    gBalloon = new Entity(BALLOON_INIT_POS, BALLOON_SIZE, BALLOON_FP);
    gBalloon->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    gDigitsTexture = LoadTexture(FUEL_DIGITS_FP);
    gFuelLabelTexture = LoadTexture(FUEL_LABEL_FP);

    gSky = new Entity(ORIGIN, SKY_SIZE, SKY_FP);
    gWater = new Entity(WATER_POS, WATER_SIZE, WATER_FP);

    gIsland = new Entity(ISLAND_POS, ISLAND_SIZE, ISLAND_FP);
    gIsland->setColliderDimensions(ISLAND_COLLIDER);
    
    gRaft = new Entity({RAFT_CENTER_X, RAFT_Y}, RAFT_SIZE, RAFT_FP);
    gRaft->setColliderDimensions(RAFT_COLLIDER);

    SetTargetFPS(FPS);
}

void processInput()
{
    Vector2 acceleration = { 0.0f, ACCELERATION_OF_GRAVITY };

    gIsBurning = false;
    gIsVenting = false;

    bool fuelFull = gFuel > 0.0f;

    if (fuelFull && (IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE))){
        acceleration.y -= BURNER_ACCELERATION;
        gIsBurning = true;
    }
        
    if (fuelFull && IsKeyDown(KEY_A)){
        acceleration.x = -VENT_ACCELERATION;
        gIsVenting = true;
    } 

    else if (fuelFull && IsKeyDown(KEY_D)){
        acceleration.x =  VENT_ACCELERATION;
        gIsVenting = true;
    }
    else{
        acceleration.x = -gBalloon->getVelocity().x * AIR_DRAG;
    }

    gBalloon->setAcceleration(acceleration);
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    gBalloon->update(deltaTime, nullptr, 0);

    if (gIsBurning) gFuel -= BURNER_FUEL_RATE * deltaTime;
    if (gIsVenting) gFuel -= VENT_FUEL_RATE * deltaTime;
    if (gFuel < 0.0f) gFuel = 0.0f;

    restrictBalloon();

    gRaftTime += deltaTime;
    float raftX = RAFT_CENTER_X + RAFT_RANGE * sin(RAFT_SPEED * gRaftTime);
    gRaft->setPosition({raftX, RAFT_Y});

}

void restrictBalloon(){
    Vector2 position = gBalloon->getPosition();
    Vector2 velocity = gBalloon->getVelocity();

    float halfWidth = BALLOON_SIZE.x / 2.0f;
    float halfHeight = BALLOON_SIZE.y / 2.0f;

    if (position.x - halfWidth < 0.0f){
        position.x = halfWidth;
        velocity.x = 0.0f;
    }

    if (position.x + halfWidth > SCREEN_WIDTH){
        position.x = SCREEN_WIDTH-halfWidth;
        velocity.x = 0.0f;
    }

    if (position.y - halfHeight < 0.0f){
        position.y = halfHeight;
        velocity.y = 0.0f;
    }

    gBalloon->setPosition(position);
    gBalloon->setVelocity(velocity);
}

void renderFuel(){
    Vector2 originOffset = {0.0f, 0.0f};

    Color tint = WHITE;
    if (gFuel < LOW_FUEL){
        tint = ColorFromHex(FUEL_LOW_COLOUR);
    } 

    Rectangle labelTextureArea = {
        0.0f, 0.0f,
        static_cast<float>(gFuelLabelTexture.width),
        static_cast<float>(gFuelLabelTexture.height),
    };


    Rectangle labelDestinationArea = {
        FUEL_IND_POS.x, FUEL_IND_POS.y,
        FUEL_LABEL_SIZE.x, FUEL_LABEL_SIZE.y
    };

    DrawTexturePro(gFuelLabelTexture, labelTextureArea, labelDestinationArea, originOffset, 0.0f, tint);
    
    int fuel = static_cast<int>(gFuel);
    int divisor = 1000;
    float digitsX = FUEL_IND_POS.x + FUEL_LABEL_SIZE.x + FUEL_LABEL_GAP;

    for (int i = 0; i < FUEL_DIGIT_COUNT; i++){
        int digit = (fuel/divisor) % 10;
        divisor /= 10;
        
        Rectangle digitTextureArea = getUVRectangle(&gDigitsTexture, digit, 1, DIGITS_COLS);

        Rectangle digitDestinationArea = {
            digitsX + i * DIGIT_SIZE.x, FUEL_IND_POS.y, DIGIT_SIZE.x, DIGIT_SIZE.y
        };

        DrawTexturePro(gDigitsTexture, digitTextureArea, digitDestinationArea, originOffset, 0.0f, tint);
    }

}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    gSky->render();
    gWater->render();
    gIsland->render();
    gRaft->render();
    gBalloon->render();
    renderFuel();

    EndDrawing();
}

void shutdown()
{
    delete gBalloon;
    delete gSky;
    delete gWater;
    delete gIsland;
    delete gRaft;
    UnloadTexture(gDigitsTexture);
    UnloadTexture(gFuelLabelTexture);
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