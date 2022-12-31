
#include <raylib.h>

static constexpr Color ballColor = { 221, 217, 218, 255 };
static constexpr Color paddleColor = { 120, 130, 122, 255 };
static constexpr Color ScoreColor = { 130, 121, 115, 255 };
static constexpr Color winnerColor = { 75, 77, 74, 255 };

class Ball
{
public:
    Ball() = default;
    
    Ball(const float& posX, const  float& posY, const float& speedX, const float& speedY, const float& radius)
        : x(posX), y(posY), speedX(speedX), speedY(speedY), radius(radius) {}
    
    // data members
    float x, y;
    float speedX, speedY;
    float radius;

    // member functions
    void Draw()
    {
        DrawCircle((int)x, (int)y, radius, ballColor);
    }

};

class Paddle
{
public:

    Paddle() = default;
    Paddle(const float& posX, const float& posY, const float& speed, const float& width, const float& height)
        : x(posX), y(posY), speed(speed), width(width), height(height)  {}

    float x, y;
    float speed;
    float width, height;
    int score = 0;

    Rectangle GetRect()
    {
        return Rectangle{ x - width / 2, y - height / 2, 10, 100 };
    }

    void Draw()
    {
        // takes a rectangle object, instead of x, y, width and height.
        DrawRectangleRec(GetRect(), paddleColor);
    }
};

class Game
{
public:

    Game() = default;

    void Init();
    void ReInit(Ball& ball);
    void Update(Ball& ball, Paddle& leftPaddle, Paddle& rightPaddle, const Sound& hitSound);
    void Draw(Ball& ball,  Paddle& leftPaddle, Paddle& rightPaddle);
    void DeInit(Sound& hitSound);
    
    bool gameOver = false;
    bool pause = false;
    const char* winnerText = nullptr;
    int rScore = 0;
    int lScore = 0;
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

void Game::ReInit(Ball& ball)
{
    this->winnerText = nullptr;
    this->rScore = 0;
    this->lScore = 0;
    ball.radius = 5;
    ball.x = GetScreenWidth() / 2.f;
    ball.y = GetScreenHeight() / 2.f;
    this->pause = false;
    this->gameOver = false;
}

void Game::Update(Ball& ball, Paddle& leftPaddle, Paddle& rightPaddle, const Sound& hitSound)
{
    // Update
    ball.x += ball.speedX * GetFrameTime();
    ball.y += ball.speedY * GetFrameTime();
    if (!gameOver)
    {

        // ball collision
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

        if (ball.x < 0)
        {
            rScore += 1;
            pause = true;

            if (rScore == 5)
            {
                gameOver = true;
                winnerText = "Right Player Wins!";
                ball.radius = 0;
            }
        }

        if (ball.x > GetScreenWidth())
        {
            lScore += 1;
            pause = true;
            if (lScore == 5)
            {
                gameOver = true;
                winnerText = "Left Player Wins!";
                ball.radius = 0;
            }
        }

        // paddles - wall collision
        const int minPaddleY = 0 + rightPaddle.height / 2 + 25;
        if (rightPaddle.y <= minPaddleY) rightPaddle.y = minPaddleY;
        if (leftPaddle.y <= minPaddleY) leftPaddle.y = minPaddleY;
        
        const int maxPaddleY = GetScreenHeight() - rightPaddle.height / 2 - 25; 
        if (rightPaddle.y >= maxPaddleY) rightPaddle.y = maxPaddleY;  
        if (leftPaddle.y >= maxPaddleY) leftPaddle.y = maxPaddleY;  

        // input 
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

        if (pause)
        {
            ball.x = GetScreenWidth() / 2.f;
            ball.y = GetScreenHeight() / 2.f;
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            if (!gameOver)
            {
                pause = !pause;
            }
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            ReInit(ball);
        }
    }
}

void Game::Draw(Ball& ball, Paddle& leftPaddle, Paddle& rightPaddle)
{
    // Draw 
    BeginDrawing();
    ClearBackground(BLACK);
        
    ball.Draw();
    leftPaddle.Draw();
    rightPaddle.Draw();
    
    DrawText(TextFormat("Score: [%i]", lScore), leftPaddle.height, 10, 30, DARKGREEN);
    DrawText(TextFormat("Score: [%i]", rScore), GetScreenWidth() - 240, 10, 30, DARKGREEN);

    if (pause && !gameOver)
    {
        int textWidth = MeasureText("Press Enter to Continue..", 40);
        DrawText("Press Enter to Continue..", GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2.5 - 30, 40, DARKGREEN);
    }

    if (winnerText)
    {
        int textWidth = MeasureText(winnerText, 60);
        DrawText(winnerText, GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 3 - 30, 60, winnerColor);
        DrawText("Press Enter To Restart The Game", GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 1.5 - 30, 30, DARKGREEN);
    }
    
    EndDrawing();
}

void Game::DeInit(Sound& hitSound)
{
    // De-Initialization
    StopSoundMulti();
    UnloadSound(hitSound);
    CloseAudioDevice();
    CloseWindow();
}


int main(void)
{
    Game Game;
    Game.Init();

    Sound hitSound = LoadSound("SFX/PongHIT.wav");
    
    // ball
    Ball ball(GetScreenWidth() / 2.f, GetScreenHeight() / 2.f, 150, 150, 5);

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

