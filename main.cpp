#include "raylib.h"
#include <vector>
#include <cmath>
#include <ctime>
#include <fstream>
#include <string>
#include <algorithm>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

struct Ball {
    float x, y;
    float speedX, speedY;
    float radius;
};

struct Paddle {
    float x, y;
    float width, height;
    float speed;
};

struct Particle {
    Vector2 position;
    Color color;
    float alpha;
};

struct PowerUp {
    float x, y;
    float radius;
    bool active;
};

void DrawParticles(std::vector<Particle>& particles) {
    for (auto& p : particles) {
        DrawCircleV(p.position, 2, Fade(p.color, p.alpha));
        p.alpha -= 0.01f;
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(), [](Particle& p){ return p.alpha <= 0; }), particles.end());
}

void SaveScore(int player1Score, int player2Score, bool vsCPU) {
    std::ofstream file("scores.txt", std::ios::app);
    if (file.is_open()) {
        if (vsCPU)
            file << "Player: " << player1Score << " - CPU: " << player2Score << "\n";
        else
            file << "Player 1: " << player1Score << " - Player 2: " << player2Score << "\n";
        file.close();
    }
}

void ResetGame(Ball& ball, Paddle& player1, Paddle& player2, int& player1Score, int& player2Score, bool& gameOver, bool& paused) {
    ball = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 7, 7, 10 };
    player1 = { 50, SCREEN_HEIGHT / 2.0f - 60, 20, 120, 7 };
    player2 = { SCREEN_WIDTH - 70, SCREEN_HEIGHT / 2.0f - 60, 20, 120, 6 };
    player1Score = player2Score = 0;
    gameOver = false;
    paused = false;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Full Game with Sound");
    ToggleFullscreen();
    InitAudioDevice();
    SetTargetFPS(60);
    HideCursor();
    srand(time(0));

    Sound sPaddleHit = LoadSound("resure/04.wav");
    Sound sScore = LoadSound("resure/02.wav");
    Sound sPowerUp = LoadSound("resure/powerup.wav");
    Sound sPowerUpSpawn = LoadSound("resure/powerup.wav");

    std::vector<Sound> gameOverSounds = {
        LoadSound("resure/1-000-000-000-iq.mp3"),
        LoadSound("resure/am-pofta-de-baut.mp3"),
        LoadSound("resure/ataca-un-hacker.mp3"),
        LoadSound("resure/bai-muie.mp3"),
        LoadSound("resure/date-ca-ma-cac.mp3"),
        LoadSound("resure/gagicar-ca-asa-vreau-eu.mp3"),
        LoadSound("resure/hai-mai-repede.mp3"),
        LoadSound("resure/ia-zi-bit-mergem-sa-manancam.mp3"),
        LoadSound("resure/ilie-nervosu-fdumnezei.mp3"),
        LoadSound("resure/intra-n-apa-marii-earrape.mp3"),
        LoadSound("resure/omni-man-are-you-sure.mp3"),
        LoadSound("resure/pumnu-meu-beton-armat.mp3"),
        LoadSound("resure/scandura-te-chema-ma-ta.mp3")
    };

    Ball ball = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 7, 7, 10 };
    Paddle player1 = { 50, SCREEN_HEIGHT / 2.0f - 60, 20, 120, 7 };
    Paddle player2 = { SCREEN_WIDTH - 70, SCREEN_HEIGHT / 2.0f - 60, 20, 120, 6 };

    PowerUp powerUp = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 12, true };
    float powerUpTimer = 0;
    bool powerUpActive = false;

    std::vector<Particle> particles;

    int player1Score = 0, player2Score = 0;
    bool paused = false, gameStarted = false, gameOver = false;
    bool vsCPU = true;

    enum Difficulty { EASY, MEDIUM, HARD };
    Difficulty difficulty = MEDIUM;

    while (!WindowShouldClose()) {
        if (!gameStarted) {
            if (IsKeyPressed(KEY_ENTER)) gameStarted = true;
            if (IsKeyPressed(KEY_ONE)) vsCPU = false;
            if (IsKeyPressed(KEY_TWO)) vsCPU = true;
            if (IsKeyPressed(KEY_E)) difficulty = EASY;
            if (IsKeyPressed(KEY_M)) difficulty = MEDIUM;
            if (IsKeyPressed(KEY_H)) difficulty = HARD;

            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("PONG GAME", SCREEN_WIDTH / 2 - 100, 100, 40, WHITE);
            DrawText("Press 1 for Player vs Player", SCREEN_WIDTH / 2 - 150, 200, 20, GRAY);
            DrawText("Press 2 for Player vs CPU", SCREEN_WIDTH / 2 - 150, 230, 20, GRAY);
            DrawText("E - Easy | M - Medium | H - Hard", SCREEN_WIDTH / 2 - 160, 300, 20, GRAY);
            DrawText("Press ENTER to Start", SCREEN_WIDTH / 2 - 150, 400, 30, WHITE);
            EndDrawing();
            continue;
        }

        if (paused) {
            if (IsKeyPressed(KEY_P)) paused = false;
            if (IsKeyPressed(KEY_R)) {
                ResetGame(ball, player1, player2, player1Score, player2Score, gameOver, paused);
            }
            if (IsKeyPressed(KEY_ESCAPE)) break;
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Paused", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 40, 40, WHITE);
            DrawText("Press P to Resume", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 10, 20, GRAY);
            DrawText("R - Restart | ESC - Quit", SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2 + 40, 20, GRAY);
            EndDrawing();
            continue;
        }

        if (IsKeyPressed(KEY_P)) paused = true;

        if (!gameOver) {
            if (IsKeyDown(KEY_W)) player1.y -= player1.speed;
            if (IsKeyDown(KEY_S)) player1.y += player1.speed;

            if (vsCPU) {
                float aiSpeed = (difficulty == EASY) ? 3 : (difficulty == MEDIUM ? 5 : 7);
                if (ball.y > player2.y + player2.height / 2) player2.y += aiSpeed;
                else player2.y -= aiSpeed;
            } else {
                if (IsKeyDown(KEY_UP)) player2.y -= player2.speed;
                if (IsKeyDown(KEY_DOWN)) player2.y += player2.speed;
            }

            ball.x += ball.speedX;
            ball.y += ball.speedY;

            if (ball.y < 0 || ball.y > SCREEN_HEIGHT) ball.speedY *= -1;

            if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { player1.x, player1.y, player1.width, player1.height })) {
                ball.speedX *= -1;
                PlaySound(sPaddleHit);
            }

            if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { player2.x, player2.y, player2.width, player2.height })) {
                ball.speedX *= -1;
                PlaySound(sPaddleHit);
            }

            if (ball.x < 0) {
                player2Score++;
                ball = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, -7, 7, 10 };
                PlaySound(sScore);
            }

            if (ball.x > SCREEN_WIDTH) {
                player1Score++;
                ball = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 7, 7, 10 };
                PlaySound(sScore);
            }

            if (fabs(ball.speedX) > 6)
                particles.push_back({ {ball.x, ball.y}, YELLOW, 1.0f });

            if (powerUp.active && CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { powerUp.x - powerUp.radius, powerUp.y - powerUp.radius, powerUp.radius * 2, powerUp.radius * 2 })) {
                player1.height = 180;
                powerUp.active = false;
                powerUpTimer = GetTime();
                PlaySound(sPowerUp);
            }

            if (!powerUp.active && GetTime() - powerUpTimer > 8.0f) {
                powerUp = { GetRandomValue(200, SCREEN_WIDTH - 200), GetRandomValue(100, SCREEN_HEIGHT - 100), 12, true };
                player1.height = 120;
                PlaySound(sPowerUpSpawn);
            }

            if (player1Score >= 5 || player2Score >= 5) {
                SaveScore(player1Score, player2Score, vsCPU);
                gameOver = true;
                int randomIndex = GetRandomValue(0, gameOverSounds.size() - 1);
                PlaySound(gameOverSounds[randomIndex]);
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                ResetGame(ball, player1, player2, player1Score, player2Score, gameOver, paused);
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleRec({ player1.x, player1.y, player1.width, player1.height }, WHITE);
        DrawRectangleRec({ player2.x, player2.y, player2.width, player2.height }, WHITE);
        DrawCircle(ball.x, ball.y, ball.radius, RED);
        if (powerUp.active) DrawCircle(powerUp.x, powerUp.y, powerUp.radius, BLUE);
        DrawParticles(particles);
        DrawText(TextFormat("%d", player1Score), SCREEN_WIDTH / 4, 20, 40, WHITE);
        DrawText(TextFormat("%d", player2Score), SCREEN_WIDTH * 3 / 4, 20, 40, WHITE);
        if (gameOver) {
            DrawText("Game Over", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 30, 40, RED);
            DrawText("Press R to Restart | ESC to Quit", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 20, 20, WHITE);
        }
        EndDrawing();
    }

    for (auto& sound : gameOverSounds) {
        UnloadSound(sound);
    }
    UnloadSound(sPaddleHit);
    UnloadSound(sScore);
    UnloadSound(sPowerUp);
    UnloadSound(sPowerUpSpawn);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}