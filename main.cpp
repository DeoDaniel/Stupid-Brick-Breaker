#include <iostream>
#include <raylib.h>
#include <string>
#include <vector>
#include "raylib-cpp/include/raylib-cpp.hpp"
using namespace std;

int screenWidth = 400;
int screenHeight = 600;

// Game state enum
enum GameState {
    MENU,
    TRANSITIONING,
    PLAYING,
    GAME_OVER
};

GameState gameState = MENU;
float transitionTime = 0.0f;
const float TRANSITION_DURATION = 1.0f;  // Duration of slide transition in seconds
bool paused = false;
vector<raylib::Rectangle> bricks;

int score = 0;  // Score tracker

bool x_positive = true;
bool x_negative = true;

bool clipping;

int btnState = 0;               // Button state: 0-NORMAL, 1-MOUSE_HOVER, 2-PRESSED
bool btnAction = false;         // Button action should be activated

// Mouse dragging variables
Vector2 mousePoint = { 0.0f, 0.0f };
Vector2 lastMousePoint = { 0.0f, 0.0f };
Vector2 delta = Vector2Subtract(mousePoint, lastMousePoint);
bool isDragging = false;

// Ball properties
Vector2 ballPos = { 180,-140 };
float initial_speed = 3;
float speed_increment = 0.1;
float ball_speed = initial_speed;

int x_factor = 0;
int y_factor = ball_speed;

void moveWindow(int key) {
    if (key == KEY_W) {
        SetWindowPosition(GetWindowPosition().x, GetWindowPosition().y - 5);
    }
    else if (key == KEY_S) {
        SetWindowPosition(GetWindowPosition().x, GetWindowPosition().y + 5);
    }
    else if (key == KEY_A && x_positive && GetWindowPosition().x) {
        SetWindowPosition(GetWindowPosition().x - 5, GetWindowPosition().y);
    }
    else if (key == KEY_D && x_negative && GetWindowPosition().x) {
        SetWindowPosition(GetWindowPosition().x + 5, GetWindowPosition().y);
    }
}

void keyInput(raylib::Rectangle direction, int key, Color color1, Color color2) {
        if (IsKeyDown(key) && gameState == PLAYING) {
            direction.Draw(color2);
            moveWindow(key);
        }
        else if (IsKeyUp(key) && gameState == PLAYING) {
            direction.Draw(color1);
        }
    }

// Function to remove a brick when hit by the ball
void removeBrick(int index) {
    if (index >= 0 && index < bricks.size()) {
        bricks.erase(bricks.begin() + index);
        score += 100/8;
    }
}

void showScore() {
    string scoreText = "Score: " + to_string(score);
    int scoreTextWidth = MeasureText(scoreText.c_str(), 20);
    DrawText(scoreText.c_str(), screenWidth/2 - scoreTextWidth/2, 45, 20, WHITE);
}

void finalScore() {
    string scoreText = "Score: " + to_string(score);
    int scoreTextWidth = MeasureText(scoreText.c_str(), 40);
    DrawText(scoreText.c_str(), screenWidth/2 - scoreTextWidth/2, 45, 40, BLACK);
}

bool isHovering;

void drawMenu() {
    // Draw background
    ClearBackground(BLACK);
    
    // Draw title
    const char* title = "BRICK BREAKER";
    int titleWidth = MeasureText(title, 40);
    DrawText(title, screenWidth/2 - titleWidth/2, 100, 40, WHITE);
    
    const char* subtitle = "But OBNOXIOUSLY stupid :)";
    int subtitleWidth = MeasureText(subtitle, 20);
    DrawText(subtitle, screenWidth/2 - subtitleWidth/2, 160, 20, MAROON);
    
    // Base button dimensions
    float baseWidth = 150.0f;
    float baseHeight = 60.0f;
    float baseX = screenWidth/2 - baseWidth/2;
    float baseY = 300.0f;
    
    raylib::Rectangle startBtn(baseX, baseY, baseWidth, baseHeight);
    
    // Animate button scale with bouncy effect
    static float buttonScale = 1.0f;
    
    const char* btnText = "START";
    int btnTextWidth = MeasureText(btnText, 40);
    
    // Check if start button is clicked
    Vector2 mousePoint = GetMousePosition();
    if (CheckCollisionPointRec(mousePoint, startBtn)) {
        if (!isHovering) {
            PlaySound(LoadSound("src/Beep_play.wav"));
            isHovering = true;
        }
        
        float targetScale = 1.25f;
        buttonScale = Lerp(buttonScale, targetScale, 0.24f);  // Smooth lerp for bouncy feel
        
        // Calculate scaled button dimensions and position (centered)
        float scaledWidth = baseWidth * buttonScale;
        float scaledHeight = baseHeight * buttonScale;
        float scaledX = screenWidth/2 - scaledWidth/2;
        float scaledY = baseY - (scaledHeight - baseHeight)/2;  // Center vertically
        
        raylib::Rectangle scaledBtn(scaledX, scaledY, scaledWidth, scaledHeight);
        scaledBtn.Draw(DARKGREEN);
        
        // Draw text centered on the scaled button
        DrawText(btnText, screenWidth/2 - btnTextWidth/2, (int)(scaledY + scaledHeight/2 - 20), 40, BLACK);
        
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            PlaySound(LoadSound("src/Beep_confirm.wav"));
            gameState = TRANSITIONING;
            transitionTime = 0.0f;
        }
    }
    else {
        float targetScale = 1.0f;
        buttonScale = Lerp(buttonScale, targetScale, 0.24f);
        
        // Calculate scaled button dimensions and position
        float scaledWidth = baseWidth * buttonScale;
        float scaledHeight = baseHeight * buttonScale;
        float scaledX = screenWidth/2 - scaledWidth/2;
        float scaledY = baseY - (scaledHeight - baseHeight)/2;
        
        raylib::Rectangle scaledBtn(scaledX, scaledY, scaledWidth, scaledHeight);
        scaledBtn.Draw(GREEN);
        
        // Draw text centered on the scaled button
        DrawText(btnText, screenWidth/2 - btnTextWidth/2, (int)(scaledY + scaledHeight/2 - 20), 40, BLACK);
        
        isHovering = false;
    }
    
    // Draw instructions
    const char* instructions = "Use WASD / Click & Drag to move the window.";
    DrawText(instructions, 65, screenHeight - 120, 12, DARKGRAY);
    const char* note = "Press [Esc] to exit.";
    DrawText(note, 145, screenHeight - 100, 12, DARKGRAY);

    //Draw version info
    int alphaTextWidth = MeasureText("Alpha v0.1", 16);
    DrawText("Alpha v0.1", screenWidth - alphaTextWidth - 10, screenHeight - 20, 16, DARKGRAY);
}


int main() {

    raylib::Window window(screenWidth, screenHeight, "raylib-cpp - Untitled Game");
    
    InitAudioDevice();

    Sound brickHit = LoadSound("src/Beep1.wav");
    Sound wallHit = LoadSound("src/Beep2.wav");
    Sound ceilingHit = LoadSound("src/Beep3.wav");
    Sound paddleHit = LoadSound("src/Beep4.wav");
    Sound gameRestart = LoadSound("src/Game_start.wav");
    Sound gameOver = LoadSound("src/Game_end.wav");

    SetWindowState(FLAG_WINDOW_UNDECORATED);

    SetTargetFPS(60);

    Vector2 window_pos = window.GetPosition();

    // Initialize bricks
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 8; j++) {
            bricks.push_back(raylib::Rectangle(3 + i*40, 100 + j*15, 34, 10));
        }
    }

    raylib::Rectangle closeButton(screenWidth - 30, 10, 20, 20);
    
    int centered_x = window.GetPosition().GetX() + screenWidth/2;

    while (!window.ShouldClose())
    {
        // Handle transition state
        if (gameState == TRANSITIONING) {
            transitionTime += GetFrameTime();
            float progress = Clamp(transitionTime / TRANSITION_DURATION, 0.0f, 1.0f);
            
            // Calculate slide offsets using easing
            float easeProgress = progress;  // Linear easing
            int menuSlideX = (int)(-screenWidth * easeProgress);
            int gameSlideX = (int)(screenWidth * (1.0f - easeProgress));
            
            BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw menu sliding out to the left
            DrawText("BRICK BREAKER", screenWidth/2 - MeasureText("BRICK BREAKER", 40)/2 + menuSlideX, 100, 40, WHITE);
            DrawText("But OBNOXIOUSLY stupid :)", screenWidth/2 - MeasureText("But OBNOXIOUSLY stupid :)", 20)/2 + menuSlideX, 160, 20, MAROON);
            
            raylib::Rectangle startBtn(screenWidth/2 - 75 + menuSlideX, 300, 150, 60);
            startBtn.Draw(GREEN);
            DrawText("START", screenWidth/2 - MeasureText("START", 40)/2 + menuSlideX, 310, 40, BLACK);
            
            // Draw game starting to appear from the right
            raylib::Rectangle ball(gameSlideX + 190, 300, 10, 10);
            ball.Draw(WHITE);
            
            raylib::Rectangle paddle(gameSlideX + 160, 540, 80, 15);
            paddle.Draw(WHITE);
            
            // Draw all bricks with fade in effect and slide offset
            for (int i = 0; i < bricks.size(); i++) {
                Color brickColor = WHITE;
                brickColor.a = (unsigned char)(255 * easeProgress);  // Fade in effect
                raylib::Rectangle offsetBrick(bricks[i].GetX() + gameSlideX, bricks[i].GetY(), bricks[i].GetWidth(), bricks[i].GetHeight());
                offsetBrick.Draw(brickColor);
            }
            
            DrawText("Score: 0", screenWidth/2 - MeasureText("Score: 0", 20)/2 + gameSlideX, 45, 20, WHITE);
            
            EndDrawing();
            
            if (progress >= 1.0f) {
                PlaySound(LoadSound("src/Game_start.wav"));
                gameState = PLAYING;
            }
            continue;
        }
        
        // Handle menu state
        if (gameState == MENU) {
            BeginDrawing();
            drawMenu();
            EndDrawing();
            continue;
        }
        
        int fixed_x = window.GetPosition().GetX()*-1 + screenWidth+380;
        int fixed_y = window.GetPosition().GetY()*-1 + screenHeight+50;

        int y_pos = window.GetPosition().GetY();

        mousePoint = GetMousePosition();
        btnAction = false;

        // Check button state
        if (CheckCollisionPointRec(mousePoint, closeButton))
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) btnState = 2;
            else btnState = 1;

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) btnAction = true;
        }
        else btnState = 0;

        if (btnAction) {
            CloseWindow();
        }

        // Handle window dragging with mouse
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(mousePoint, closeButton) && gameState == PLAYING) {
            isDragging = true;
        }
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isDragging = false;
        }

        if (isDragging && gameState == PLAYING) {
            delta = Vector2Subtract(mousePoint, lastMousePoint);
            cout << mousePoint.x << ", " << mousePoint.y << endl;
            if (x_positive == false) {
                delta.x = max(delta.x, 0.0f);  // Prevent moving window left
            }
            if (x_negative == false) {
                delta.x = min(delta.x, 0.0f);  // Prevent moving window right
            }
            if (delta.x != 0 || delta.y != 0) {
                SetWindowPosition(int(GetWindowPosition().x + int(delta.x)), int(GetWindowPosition().y + delta.y));
                
            }
        }
        else {
            lastMousePoint = mousePoint;
            SetWindowPosition(GetWindowPosition().x, GetWindowPosition().y);
        }

        if (gameState == PLAYING) {
            ballPos.x += x_factor;
            ballPos.y += y_factor;
        }

        BeginDrawing();

        // Draw game objects
        raylib::Rectangle ball(fixed_x + centered_x - 960 + ballPos.x, fixed_y + ballPos.y, 10, 10);
        raylib::Rectangle paddle(fixed_x + centered_x - 820, 540, 80, 15);
        paddle.Draw(WHITE);
        
        // Draw bricks
        for (int i = 0; i < bricks.size(); i++) {
            bricks[i].Draw(WHITE);
        }

        // Ball collision with bricks
        for (int i = 0; i < bricks.size(); i++) {
            raylib::Rectangle& brick = bricks[i];
            if (CheckCollisionRecs(ball, brick)) {

                PlaySound(brickHit);
                ball_speed += speed_increment; // Optional: increase speed on hit
                
                // Determine which side of brick was hit
                float brick_top = brick.GetY();
                float brick_bottom = brick.GetY() + brick.GetHeight();
                float brick_left = brick.GetX();
                float brick_right = brick.GetX() + brick.GetWidth();
                
                float ball_center_x = ball.GetX() + ball.GetWidth() / 2;
                float ball_center_y = ball.GetY() + ball.GetHeight() / 2;
                
                // Check which edge is closest
                float dist_top = abs(ball_center_y - brick_top);
                float dist_bottom = abs(ball_center_y - brick_bottom);
                float dist_left = abs(ball_center_x - brick_left);
                float dist_right = abs(ball_center_x - brick_right);
                
                float min_dist = min({dist_top, dist_bottom, dist_left, dist_right});
                
                if (min_dist == dist_top || min_dist == dist_bottom) {
                    y_factor = -y_factor;
                } else {
                    x_factor = -x_factor;
                }
                
                // Remove the brick that was hit
                removeBrick(i);
                break;  // Exit loop to avoid issues with vector modification
            }
        }

        // Pause events handling
        if (gameState == PLAYING) { 
            window.ClearBackground(BLACK);
            ball.Draw(WHITE);
            showScore();
        }
        else if (gameState == GAME_OVER) {
            window.ClearBackground(MAROON);
            window.SetPosition(window_pos);
            ballPos = Vector2{180, 0};
            paddle.Draw(BLACK);
            DrawText("GAME OVER", screenWidth/2 - MeasureText("GAME OVER", 40)/2, screenHeight/2 - 20, 40, BLACK);
            DrawText("Press [SPACE] to Restart", screenWidth/2 - MeasureText("Press [SPACE] to Restart", 20)/2, screenHeight/2 + 30, 20, BLACK);
            finalScore();
            // Black bricks
            for (int i = 0; i < bricks.size(); i++) {
                bricks[i].Draw(BLACK);
            }
            // Restart game on SPACE key press
            if (IsKeyPressed(KEY_SPACE)) {
                ball_speed = initial_speed;
                score = 0;  // Reset score
                x_factor = 0;
                y_factor = ball_speed;
                bricks.clear();
                // Reinitialize bricks
                for (int i = 0; i < 10; i++) {
                    for (int j = 0; j < 8; j++) {
                    bricks.push_back(raylib::Rectangle(3 + i*40, 100 + j*15, 34, 10));
                }
                gameState = PLAYING;
                PlaySound(gameRestart);
            }
        }
    }
        

        // Handle window movement input
        raylib::Rectangle up(10, screenHeight-20, 10, 10);
        keyInput(up, KEY_W, GREEN, DARKGREEN);
        raylib::Rectangle down(10, screenHeight-10, 10, 10);
        keyInput(down, KEY_S, RED, Color{117, 41, 55, 255});
        raylib::Rectangle left(0, screenHeight-10, 10, 10);
        keyInput(left, KEY_A, BLUE, DARKBLUE);
        raylib::Rectangle right(20, screenHeight-10, 10, 10);
        keyInput(right, KEY_D, ORANGE, BROWN);


        // Draw GUIs
        raylib::Rectangle menuBar(0, 0, screenWidth, 40);
        menuBar.Draw(DARKGRAY);
        if (CheckCollisionPointRec(mousePoint, closeButton))
        {
            closeButton.Draw(MAROON);
            DrawText("X", screenWidth - 26, 12, 18, WHITE);
        }
        else {
            closeButton.Draw(RED);
            DrawText("X", screenWidth - 26, 12, 18, LIGHTGRAY);
        }
        
        DrawText("Brick Breaker but STUPID :)", 95, 11, 19, LIGHTGRAY);
        window.DrawFPS(10, 10);

        int alphaTextWidth = MeasureText("Alpha v0.1", 16);
        if (gameState == PLAYING) {
            DrawText("Alpha v0.1", screenWidth - alphaTextWidth - 10, screenHeight - 20, 16, DARKGRAY);

        }
        
        // Draw score at the middle top of the window
        


        EndDrawing();


        // More Position Debugging
        //cout << ball.GetPosition().x - fixed_x << ", " << ball.GetPosition().y - fixed_y << endl;
        //cout << paddle.GetPosition().x - fixed_x << ", " << paddle.GetPosition().y - fixed_y << endl;
        

        // Ball movement and collision
        if (ball.GetPosition().y >= screenHeight && gameState == PLAYING) {
            PlaySound(gameOver); 
            gameState = GAME_OVER;
        }
        // Paddle with ball collision detection
        else if (ball.GetPosition().y + ball.GetHeight() >= paddle.GetPosition().GetY() &&
                 ball.GetPosition().y + ball.GetHeight() <= paddle.GetPosition().GetY() + paddle.GetHeight() &&
                 ball.GetPosition().x + ball.GetWidth() > paddle.GetPosition().GetX() &&
                 ball.GetPosition().x < paddle.GetPosition().GetX() + paddle.GetWidth()) {

            y_factor = -ball_speed;

            if (!IsSoundPlaying(paddleHit) && gameState == PLAYING){
                PlaySound(paddleHit);
            }

            clipping = true;

            if (x_factor == 0) {    // Ball was moving straight down, give it a horizontal direction
                x_factor = ball_speed;
            }
            else if (ball.GetPosition().x + ball.GetWidth()/2 < paddle.GetPosition().GetX() + paddle.GetWidth()/2) {    // Ball hit left side of paddle
                x_factor = -ball_speed;
            }
            else if (ball.GetPosition().x + ball.GetWidth()/2 > paddle.GetPosition().GetX() + paddle.GetWidth()/2) {    // Ball hit right side of paddle
                x_factor = ball_speed;
            }
        }
        else if (ball.GetPosition().y < menuBar.GetHeight()) {
            if (!IsSoundPlaying(ceilingHit) && gameState == PLAYING){
                PlaySound(ceilingHit);
            }
            clipping = true;
            ball.SetPosition(Vector2{ball.GetPosition().x, menuBar.GetHeight()});
            y_factor = ball_speed;
        }


        // Window edges with ball collision detection
        if (ball.GetPosition().x >= screenWidth - ball.GetWidth()) {
            if (!IsSoundPlaying(wallHit) && !clipping){
                PlaySound(wallHit);
            }
            clipping = true;
            ball.SetPosition(Vector2{screenWidth - ball.GetWidth(), ball.GetPosition().y});
            x_factor = -ball_speed;
        }
        else if (ball.GetPosition().x <= 0) {
            if (!IsSoundPlaying(wallHit) && !clipping){
                PlaySound(wallHit);
            }
            clipping = true;
            ball.SetPosition(Vector2{0, ball.GetPosition().y});
            x_factor = ball_speed;
        }
        else {
            clipping = false;
        }


        // Window edges with paddle collision detection
        if (paddle.GetPosition().GetX() > screenWidth - paddle.GetWidth() - 10) {
            x_positive = false;
            if (paddle.GetPosition().GetX() > screenWidth - paddle.GetWidth()) {
                SetWindowPosition(window_pos.x - screenWidth/2 + paddle.GetWidth()/2, GetWindowPosition().y);
            }
        }
        else if (paddle.GetPosition().GetX() < 10) {
            x_negative = false;
            if (paddle.GetPosition().GetX() < 0) {
                SetWindowPosition(window_pos.x + screenWidth/2 - paddle.GetWidth()/2, GetWindowPosition().y);
            }
        }
        else {
            x_positive = true;
            x_negative = true;
        }
        
    }

    // Unload sound data.
    UnloadSound(brickHit);
    UnloadSound(wallHit);
    UnloadSound(ceilingHit);
    UnloadSound(paddleHit);
    UnloadSound(gameRestart);
    UnloadSound(gameOver);

    CloseAudioDevice();

    // UnloadTexture() and CloseWindow() are called automatically.

    return 0;
}
