#include <iostream>
#include <raylib.h>

using namespace std;
Color Green = Color{38,185,154,255};
Color Dark_Green = Color{20,160,133,255};
Color Light_Green = Color{129,204,184,255};
Color Yellow = Color{243,213,91,255};


int player_score = 0;
int cpu_score = 0;
class Ball
{
public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    void Draw()
    {
        DrawCircle(x, y, radius, Yellow);
    }
    void Update()
    {
        x += speed_x;
        y += speed_y;
        if (y + radius >= GetScreenHeight() || y - radius <= 0)
        {
            speed_y *= -1;
        }
        if (x + radius >= GetScreenWidth()) //cpu castiga
        {
            cpu_score++;
            ResetBall();
        }

        if( x - radius <= 0)//player castiga
        {
            player_score++;
            ResetBall();
        }
        
    }
    void ResetBall()
    {
        x=GetScreenWidth()/2;
        y=GetScreenHeight()/2;
        int speed_choices[2]={-1,1};
        speed_x *=speed_choices[GetRandomValue(0,1)];
        speed_y *=speed_choices[GetRandomValue(0,1)];
    }
};
class Paddle
{


protected:
    void LimitMovment()
    {
        if (y <= 0)
        {
            y = 0;
        }
        if (y + height >= GetScreenHeight())
        {
            y = GetScreenHeight() - height;
        }

    }
public:
    float x, y;
    float width, height;
    int speed;

    void Draw()
    {
        DrawRectangle(x, y, width, height, WHITE);
    }
    void Update()
    {
        if (IsKeyDown(KEY_UP))
        {
            y = y - speed;
        }
        if (IsKeyDown(KEY_DOWN))
        {
            y = y + speed;
        }
        if(IsKeyDown(KEY_LEFT)){
            speed--;
        }
        if(IsKeyDown(KEY_RIGHT)){
            speed++;
        }
        LimitMovment();
    }
};

class CpuPaddle : public Paddle
{
public:
    void Update(int ball_y)
    {
        if (y + height / 2 > ball_y)
        {
            y = y - speed;
        }
        if (y + height / 2 <= ball_y)
        {
            y = y + speed;
        }
        LimitMovment();
    }
};
Ball ball;
Paddle player;
CpuPaddle cpu;

int main()
{
    const int initial_width = 1280;
    const int initial_height = 800;
    
    // Start in fullscreen mode
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(initial_width, initial_height, "Retro Pong!");
    ToggleFullscreen();
    SetTargetFPS(60);

    // Initialize game objects with relative positions
    ball.radius = 20;
    ball.x = GetScreenWidth() / 2;
    ball.y = GetScreenHeight() / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;

    player.width = 25;
    player.height = 120;
    player.x = GetScreenWidth() - player.width - 10;
    player.y = GetScreenHeight() / 2 - player.height / 2;
    player.speed = 6;

    cpu.height = 120;
    cpu.width = 25;
    cpu.x = 10;
    cpu.y = GetScreenHeight() / 2 - cpu.height / 2;
    cpu.speed = 6;

    while (WindowShouldClose() == false)
    {
        // Toggle fullscreen/windowed mode when Escape is pressed
        if (IsKeyPressed(KEY_ESCAPE))
        {
            ToggleFullscreen();
            
            // Reset positions when changing modes
            ball.x = GetScreenWidth() / 2;
            ball.y = GetScreenHeight() / 2;
            player.x = GetScreenWidth() - player.width - 10;
            player.y = GetScreenHeight() / 2 - player.height / 2;
            cpu.y = GetScreenHeight() / 2 - cpu.height / 2;
        }

        BeginDrawing();

        // Updating
        ball.Update();
        player.Update();
        cpu.Update(ball.y);
        //verific coliziunea cu preretii

        if(CheckCollisionCircleRec(Vector2{ball.x, ball.y},ball.radius,Rectangle{player.x, player.y, player.width, player.height}))
        {
            ball.speed_x *=-1;
        }
        if(CheckCollisionCircleRec(Vector2{ball.x, ball.y},ball.radius,Rectangle{cpu.x, cpu.y, cpu.width, cpu.height}))
        {
            ball.speed_x *=-1;
        }

        // Drawing
        ClearBackground(Dark_Green);
        DrawRectangle(GetScreenWidth()/2, 0, GetScreenWidth()/2, GetScreenWidth(), Green);
        DrawCircle(GetScreenWidth()/2, GetScreenWidth()/3, 150, Light_Green);
        DrawLine(GetScreenWidth() / 2, 0, GetScreenWidth() / 2, GetScreenHeight(), WHITE);
        ball.Draw();
        cpu.Draw();
        player.Draw();
        DrawText(TextFormat("%i", cpu_score), GetScreenWidth()/4 -20, 20, 70, WHITE);
        DrawText(TextFormat("%i", player_score), 3*GetScreenWidth()/4 -20, 20, 70, WHITE);
        DrawText(TextFormat("Speed: %i", player.speed), 3.25*GetScreenWidth()/4 -20, 20, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}