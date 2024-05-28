#include "raylib.h"

#define TOP_BORDER_WIDTH 40
#define SIDE_BORDER_WIDTH 5
#define MAX_OBSTACLES 15
#define MAX_OBSTACLES_YELLOW 20
#define MAX_OBSTACLES_RED 25
#define MAX_OBSTACLES_BLACK 30
#define MAX_HITS 3
#define FLASH_DURATION 15

typedef struct Obstacle {
    Vector2 position;
    float radius;
    float speed;
} Obstacle;

void InitObstacle(Obstacle *obstacle, int screenWidth, int screenHeight) {
    obstacle->position.x = GetRandomValue(SIDE_BORDER_WIDTH, screenWidth - SIDE_BORDER_WIDTH - 40);
    obstacle->position.y = TOP_BORDER_WIDTH + 40;
    obstacle->radius = GetRandomValue(20, 40);
    obstacle->speed = GetRandomValue(5, 10);
}

bool CheckCollisionCircle(Vector2 center1, float radius1, Vector2 center2, float radius2) {
    float dx = center2.x - center1.x;
    float dy = center2.y - center1.y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance <= (radius1 + radius2);
}

void ResetGame(Vector2 *ballPosition, int screenWidth, int screenHeight, Obstacle obstacles[MAX_OBSTACLES], int *score, int *hitCount, bool *gameOver, int *flashCounter, float *timeElapsed, float *startTime, bool *countdown, int *lastLevel) {
    ballPosition->x = screenWidth / 2;
    ballPosition->y = screenHeight - 40;
    *score = 0;
    *hitCount = 0;
    *gameOver = false;
    *flashCounter = 0;
    *timeElapsed = 0.0f;
    *startTime = GetTime();
    *countdown = true;
    *lastLevel = 0;
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        InitObstacle(&obstacles[i], screenWidth, screenHeight);
    }
}

int main(void)
{
    const int screenWidth = 1600;
    const int screenHeight = 800;
    const float borderWidth = 5.0f;
    int maxObstacles = MAX_OBSTACLES; 
    int lastLevel = 0;  // Variable to track the last level where the sound was played

    InitWindow(screenWidth, screenHeight, "Dodge Game");

    // Initialize audio device
    InitAudioDevice();

    // Load sound effects
    Sound startSound = LoadSound("./sounds/startSound.wav");
    Sound hitSound = LoadSound("./sounds/damage.wav");
    Sound startHorn = LoadSound("./sounds/startSoundAfterCountdown.wav");
    Sound levelUP = LoadSound("./sounds/levelUP.wav");
    Sound gameOverSound = LoadSound("./sounds/gameover.wav");
    Sound soundTrack = LoadSound("./sounds/soundtrack.mp3");
    
    Vector2 ballPosition = { (float)screenWidth / 2, (float)screenHeight - 40 };
    const float ballRadius = 25.0f;
    int score = 0;
    int hitCount = 0;
    int highscore = 0;
    bool gameOver = false;
    int flashCounter = 0;
    Color enemiesColor = GREEN;
    float timeElapsed = 0.0f;
    float startTime = GetTime();
    bool countdown = true;

    Obstacle obstacles[MAX_OBSTACLES];
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        InitObstacle(&obstacles[i], screenWidth, screenHeight);
    }

    SetTargetFPS(60);
    PlaySound(startSound);
    while (!WindowShouldClose())
    {
        float currentTime = GetTime();
        if (!gameOver) {

            if (countdown) {

                if (currentTime - startTime >= 3.0f) { 
                    PlaySound(startHorn);
                    PlaySound(soundTrack);

                    countdown = false;
                    startTime = GetTime();
                }
            } else {
                timeElapsed += GetFrameTime();
                if (timeElapsed >= 1.0f) {
                    score++;
                    timeElapsed = 0.0f;
                }

                if (score > highscore) {
                    highscore = score;
                }

                if (score >= 25 && lastLevel < 1) {
                    PlaySound(levelUP);
                    enemiesColor = BROWN;
                    maxObstacles = MAX_OBSTACLES_YELLOW;
                    lastLevel = 1; 
                } 

                if (score >= 35 && lastLevel < 2) {
                    PlaySound(levelUP);
                    enemiesColor = RED;
                    maxObstacles = MAX_OBSTACLES_RED;
                    lastLevel = 2;
                }

                if (score >= 45 && lastLevel < 3) {
                    PlaySound(levelUP);
                    enemiesColor = BLACK;
                    maxObstacles = MAX_OBSTACLES_BLACK;
                    lastLevel = 3;
                }

                if (!IsSoundPlaying(soundTrack)) {
                    PlaySound(soundTrack);
                }


                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) ballPosition.x += 8.0f;
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) ballPosition.x -= 8.0f;
                if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) ballPosition.y -= 8.0f;
                if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) ballPosition.y += 8.0f;

                if (ballPosition.x - ballRadius < SIDE_BORDER_WIDTH) ballPosition.x = SIDE_BORDER_WIDTH + ballRadius;
                if (ballPosition.x + ballRadius > screenWidth - SIDE_BORDER_WIDTH) ballPosition.x = screenWidth - SIDE_BORDER_WIDTH - ballRadius;
                if (ballPosition.y - ballRadius < TOP_BORDER_WIDTH) ballPosition.y = TOP_BORDER_WIDTH + ballRadius;
                if (ballPosition.y + ballRadius > screenHeight - SIDE_BORDER_WIDTH) ballPosition.y = screenHeight - SIDE_BORDER_WIDTH - ballRadius;

                for (int i = 0; i < maxObstacles; i++) {
                    obstacles[i].position.y += obstacles[i].speed;
                    if (obstacles[i].position.y - obstacles[i].radius > screenHeight) {
                        InitObstacle(&obstacles[i], screenWidth, screenHeight);
                    }

                    if (CheckCollisionCircle(ballPosition, ballRadius, obstacles[i].position, obstacles[i].radius)) {
                        hitCount++;
                        flashCounter = FLASH_DURATION;
                        PlaySound(hitSound); 
                        InitObstacle(&obstacles[i], screenWidth, screenHeight); 
                        if (hitCount >= MAX_HITS) {
                            gameOver = true;
                            StopSound(soundTrack);
                            PlaySound(gameOverSound);
                        }
                    }
                }
            }
        } else {
            if (IsKeyPressed(KEY_SPACE)) {
                PlaySound(startSound);
                ResetGame(&ballPosition, screenWidth, screenHeight, obstacles, &score, &hitCount, &gameOver, &flashCounter, &timeElapsed, &startTime, &countdown, &lastLevel);
            }
        }

        BeginDrawing();

            if (flashCounter > 0) {
                Color lightRed = (Color){ 255, 170, 170, 255 };
                ClearBackground(lightRed);
                flashCounter--;
            } else {
                ClearBackground(RAYWHITE);
            }

            if (!gameOver) {
                DrawRectangle(0, 0, screenWidth, TOP_BORDER_WIDTH, BLACK);
                DrawRectangle(0, screenHeight - SIDE_BORDER_WIDTH, screenWidth, SIDE_BORDER_WIDTH, BLACK);
                DrawRectangle(0, 0, SIDE_BORDER_WIDTH, screenHeight, BLACK);
                DrawRectangle(screenWidth - SIDE_BORDER_WIDTH, 0, SIDE_BORDER_WIDTH, screenHeight, BLACK);

                DrawText("Movimente a bola com as setas do teclado ou WASD", 10, 10, 20, WHITE);

                char scoreText[20];
                sprintf(scoreText, "Score: %d", score);
                int scoreTextWidth = MeasureText(scoreText, 20);
                DrawText(scoreText, screenWidth - scoreTextWidth - 10, 10, 20, WHITE);

                char hitText[20];
                sprintf(hitText, "Hits: %d", hitCount);
                int hitTextWidth = MeasureText(hitText, 20);
                DrawText(hitText, screenWidth - hitTextWidth - 10, 60, 20, BLACK);

                char highscoreText[20];
                sprintf(highscoreText, "Highscore: %d", highscore);
                int highscoreTextWidth = MeasureText(highscoreText, 20);
                DrawText(highscoreText, screenWidth - highscoreTextWidth - 10, 90, 20, BLACK);

                DrawCircleV(ballPosition, ballRadius, WHITE);
                for (float i = 0; i < borderWidth; i += 0.1f) {
                    DrawCircleLines((int)ballPosition.x, (int)ballPosition.y, ballRadius + i, BLACK);
                }

                if (!countdown) {
                    for (int i = 0; i < maxObstacles; i++) {
                        DrawCircleV(obstacles[i].position, obstacles[i].radius, enemiesColor);
                    }
                }

                if (countdown) {
                    int countdownValue = 3 - (int)(currentTime - startTime);
                    char countdownText[5];
                    sprintf(countdownText, "%d", countdownValue);
                    int countdownTextWidth = MeasureText(countdownText, 40);
                    DrawText("PREPARE-SE!", screenWidth / 2 - MeasureText("PREPARE-SE!", 30) / 2, screenHeight / 3 - 10, 30, RED);
                    DrawText(countdownText, screenWidth / 2 - countdownTextWidth / 2, screenHeight / 2 - 20, 60, BLACK);
                }
            } else {
                enemiesColor = GREEN;
                ClearBackground(RAYWHITE);

                const char* message = "Game Over!";
                int textWidth = MeasureText(message, 45);
                DrawText(message, screenWidth / 2 - textWidth / 2, screenHeight / 2 - 50 / 2, 50, RED);

                const char* messageSpace = "Pressione ESPAÇO para jogar novamente";
                int spaceMessageWidth = MeasureText(messageSpace, 25);
                DrawText(messageSpace, screenWidth / 2 - spaceMessageWidth / 2, screenHeight / 2 + 25 / 2 + 10, 25, BLACK);

                char scoreMessage[40];
                sprintf(scoreMessage, "Seu Score: %d", score);
                int scoreMessageWidth = MeasureText(scoreMessage, 20);
                DrawText(scoreMessage, screenWidth / 2 - scoreMessageWidth / 2, screenHeight / 2 + 70, 20, BLACK);

                char highscoreMessage[40];
                sprintf(highscoreMessage, "Highscore: %d", highscore);
                int highscoreMessageWidth = MeasureText(highscoreMessage, 20);
                DrawText(highscoreMessage, screenWidth / 2 - highscoreMessageWidth / 2, screenHeight / 2 + 90, 20, BLACK);
            }

        EndDrawing();
    }

    // Unload sound effects
    UnloadSound(hitSound);
    UnloadSound(startSound);
    UnloadSound(startHorn);
    UnloadSound(levelUP);
    UnloadSound(gameOverSound);
    // Close audio device
    CloseAudioDevice();

    CloseWindow();
    return 0;
}
