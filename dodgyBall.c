#include "raylib.h"

#define TOP_BORDER_WIDTH 40
#define SIDE_BORDER_WIDTH 5
#define MAX_OBSTACLES 15
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

void ResetGame(Vector2 *ballPosition, int screenWidth, int screenHeight, Obstacle obstacles[MAX_OBSTACLES], int *score, int *hitCount, bool *gameOver, int *flashCounter, float *timeElapsed, float *startTime, bool *countdown) {
    ballPosition->x = screenWidth / 2;
    ballPosition->y = screenHeight - 40;
    *score = 0;
    *hitCount = 0;
    *gameOver = false;
    *flashCounter = 0;
    *timeElapsed = 0.0f;
    *startTime = GetTime(); // Reset the start time
    *countdown = true; // Start the countdown
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        InitObstacle(&obstacles[i], screenWidth, screenHeight);
    }
}



int main(void)
{
    const int screenWidth = 1600;
    const int screenHeight = 800;
    const float borderWidth = 5.0f;
    
    InitWindow(screenWidth, screenHeight, "Jogo do Art(h)ur");
    
    Vector2 ballPosition = { (float)screenWidth / 2, (float)screenHeight - 40 };
    const float ballRadius = 25.0f;
    int score = 0;
    int hitCount = 0;
    int highscore = 0; // Adicionando a variável de highscore
    bool gameOver = false;
    int flashCounter = 0;
    Color enemiesColor = GREEN;
    float timeElapsed = 0.0f;
    float startTime = GetTime(); // Store the start time
    bool countdown = true; // Control the countdown state

    Obstacle obstacles[MAX_OBSTACLES];
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        InitObstacle(&obstacles[i], screenWidth, screenHeight);
    }

    SetTargetFPS(60);
    
    while (!WindowShouldClose())
    {
        float currentTime = GetTime(); // Get the current time

        if (!gameOver) {
            if (countdown) {
                
                if (currentTime - startTime >= 3.0f) { // Countdown finished
                    countdown = false;
                    startTime = GetTime(); // Reset start time for game timing
                }
            } else {
                timeElapsed += GetFrameTime();
                if (timeElapsed >= 1.0f) {
                    score++;
                    timeElapsed = 0.0f;
                }

                if (score > highscore) { // Atualizando o highscore se a pontuação atual for maior
                    highscore = score;
                }
                
                if(score >= 25 && score < 35){
                    enemiesColor = YELLOW;
                }
                
                if(score >= 35 && score < 45){
                    enemiesColor = RED;
                }

                if(score >= 45){
                    enemiesColor = BLACK;
                }
                
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) ballPosition.x += 8.0f;
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) ballPosition.x -= 8.0f;
                if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) ballPosition.y -= 8.0f;
                if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) ballPosition.y += 8.0f;

                if (ballPosition.x - ballRadius < SIDE_BORDER_WIDTH) ballPosition.x = SIDE_BORDER_WIDTH + ballRadius;
                if (ballPosition.x + ballRadius > screenWidth - SIDE_BORDER_WIDTH) ballPosition.x = screenWidth - SIDE_BORDER_WIDTH - ballRadius;
                if (ballPosition.y - ballRadius < TOP_BORDER_WIDTH) ballPosition.y = TOP_BORDER_WIDTH + ballRadius;
                if (ballPosition.y + ballRadius > screenHeight - SIDE_BORDER_WIDTH) ballPosition.y = screenHeight - SIDE_BORDER_WIDTH - ballRadius;
                
                
                for (int i = 0; i < MAX_OBSTACLES; i++) {
                    obstacles[i].position.y += obstacles[i].speed;
                    if (obstacles[i].position.y - obstacles[i].radius > screenHeight) {
                        InitObstacle(&obstacles[i], screenWidth, screenHeight);
                    }

                    if (CheckCollisionCircle(ballPosition, ballRadius, obstacles[i].position, obstacles[i].radius)) {
                        hitCount++;
                        flashCounter = FLASH_DURATION;
                        InitObstacle(&obstacles[i], screenWidth, screenHeight); 
                        if (hitCount >= MAX_HITS) {
                            gameOver = true; 
                        }
                    }
                }
            }
        } else {
            if (IsKeyPressed(KEY_SPACE)) {
                ResetGame(&ballPosition, screenWidth, screenHeight, obstacles, &score, &hitCount, &gameOver, &flashCounter, &timeElapsed, &startTime, &countdown);
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
                
                char highscoreText[20]; // Exibindo o highscore
                sprintf(highscoreText, "Highscore: %d", highscore);
                int highscoreTextWidth = MeasureText(highscoreText, 20);
                DrawText(highscoreText, screenWidth - highscoreTextWidth - 10, 90, 20, BLACK);
                
                DrawCircleV(ballPosition, ballRadius, WHITE);
                for (float i = 0; i < borderWidth; i += 1.0f) {
                    DrawCircleLines((int)ballPosition.x, (int)ballPosition.y, ballRadius + i, BLUE);
                }
                
                if (!countdown) { // Only draw obstacles if countdown has finished
                    for (int i = 0; i < MAX_OBSTACLES; i++) {
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
                ClearBackground(RAYWHITE); // Limpa a tela antes de mostrar a mensagem de fim de jogo
                
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
    
    UnloadSound(countdownSound);
    CloseWindow();
    return 0;
}