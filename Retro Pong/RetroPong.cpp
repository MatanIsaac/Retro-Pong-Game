
#include "raylib.h"

class Ball
{
public:
    Ball() = default;
    
    Ball(float posX, float posY, float speedX, float speedY, float radius)
        : x(posX), y(posY), speedX(speedX), speedY(speedY), radius(radius) {}
    
    // data members
    float x, y;
    float speedX, speedY;
    float radius;

    // member functions
    void Draw()
    {
        DrawCircle((int)x, (int)y, radius, WHITE);
    }

};

class Paddle
{
public:

    Paddle() = default;
    Paddle(float posX, float posY,float speed, float width, float height)
        : x(posX), y(posY), speed(speed), width(width), height(height)  {}

    float x, y;
    float speed;
    float width, height;

    Rectangle GetRect()
    {
        return Rectangle{ x - width / 2, y - height / 2, 10, 100 };
    }

    void Draw()
    {
        // takes a rectangle object, instead of x, y, width and height.
        DrawRectangleRec(GetRect(), WHITE);
    }
};

class Game
{
public:

    Game() = default;

    void Init();
    void Update(Ball& ball, Paddle& leftPaddle, Paddle& rightPaddle, Sound hitSound);
    void Draw(Ball& ball, Paddle leftPaddle, Paddle rightPaddle);
    void DeInit(Sound hitSound);
    const char* winnerText = nullptr;
};

void Game::Init()
{

    // Initialization
    const int screenWd = 800;
    const int screenHt = 600;

    // only updates as fast as the user's refresh rate
    SetWindowState(FLAG_VSYNC_HINT);

    // Set our game to run at 60 frames-per-second
    SetTargetFPS(60);

    InitWindow(screenWd, screenHt, "Pong");

    InitAudioDevice();      // Initialize audio device

}

void Game::Update(Ball& ball, Paddle& leftPaddle, Paddle& rightPaddle, Sound hitSound)
{
    // Update
    ball.x += ball.speedX * GetFrameTime();
    ball.y += ball.speedY * GetFrameTime();

    if (ball.y < 0)
    {
        ball.y = 0;
        ball.speedY *= -1;
    }

    if (ball.y > GetScreenHeight())
    {
        ball.y = (float)GetScreenHeight();
        ball.speedY *= -1;
    }

    if (IsKeyDown(KEY_W)) leftPaddle.y -= leftPaddle.speed * GetFrameTime();

    if (IsKeyDown(KEY_S)) leftPaddle.y += leftPaddle.speed * GetFrameTime();

    if (IsKeyDown(KEY_UP)) rightPaddle.y -= rightPaddle.speed * GetFrameTime();

    if (IsKeyDown(KEY_DOWN)) rightPaddle.y += rightPaddle.speed * GetFrameTime();


    // check for collisions
    if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius, leftPaddle.GetRect()))
    {
        if (ball.speedX < 0) ball.speedX *= -1.1f;

        PlaySound(hitSound);
    }

    if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius, rightPaddle.GetRect()))
    {
        if (ball.speedX > 0) ball.speedX *= -1.1f;
        PlaySound(hitSound);
    }

    if (ball.x < 0) winnerText = "Right Player Wins!";

    if (ball.x > GetScreenWidth()) winnerText = "Left Player Wins!";

}

void Game::Draw(Ball& ball, Paddle leftPaddle, Paddle rightPaddle)
{
    // Draw 
    BeginDrawing();
    ClearBackground(BLACK);

    ball.Draw();
    leftPaddle.Draw();
    rightPaddle.Draw();

    if (winnerText)
    {
        int textWidth = MeasureText(winnerText, 60);
        DrawText(winnerText, GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2 - 30, 60, RED);
    }

    DrawFPS(10, 10);

    EndDrawing();
}

void Game::DeInit(Sound hitSound)
{
    // De-Initialization
    StopSoundMulti();
    UnloadSound(hitSound);
    CloseAudioDevice();
    CloseWindow();
}

#include <iostream>

int main(void)
{
    Game Game;
    Game.Init();

    Sound hitSound = LoadSound("SFX/PongHIT.wav");
    
    // ball
    Ball ball(GetScreenWidth() / 2.f, GetScreenHeight() / 2.0f, 300, 300, 5);

    // Paddles
    Paddle leftPaddle(50, GetScreenHeight() / 2.f,500,10,100);
    Paddle rightPaddle(GetScreenWidth() - 50.f, GetScreenHeight() / 2.f,500,10,100);
   
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        Game.Update(ball, leftPaddle, rightPaddle, hitSound);
        Game.Draw(ball, leftPaddle, rightPaddle);
    }

    Game.DeInit(hitSound);

}

