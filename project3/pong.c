#include <msp430.h>
#include <lcdutils.h>
#include <lcddraw.h>

// Paddle and ball positions
int paddle1Y = screenHeight / 2 - 20;  // Initial y-coordinate for the center of paddle 1
int paddle2Y = screenHeight / 2 - 20;  // Initial y-coordinate for the center of paddle 2
int ballX = screenWidth / 2;           // Initial x-coordinate for the ball
int ballY = screenHeight / 2;          // Initial y-coordinate for the ball

// Paddle and ball dimensions
int paddleWidth = 5;
int paddleHeight = 40;
int ballSize = 5;

// Ball velocity
int ballVelocityX = 2;
int ballVelocityY = 1;

// Function to initialize the LCD and set up any necessary configurations
void lcd_init_setup() {
    // Initialization code for the LCD
    configureClocks();
    lcd_init();
}

// Function to update the game state
void update_game_state() {
    // Update paddles
    if (paddle1Y < 0) paddle1Y = 0;
    if (paddle1Y > screenHeight - paddleHeight) paddle1Y = screenHeight - paddleHeight;

    if (paddle2Y < 0) paddle2Y = 0;
    if (paddle2Y > screenHeight - paddleHeight) paddle2Y = screenHeight - paddleHeight;

    // Update ball
    ballX += ballVelocityX;
    ballY += ballVelocityY;

    // Simple ball collision with top and bottom of the screen
    if (ballY - ballSize / 2 <= 0 || ballY + ballSize / 2 >= screenHeight) {
        ballVelocityY = -ballVelocityY;  // Reverse the vertical velocity
    }

    // Ball collision with paddles
    if ((ballX - ballSize / 2 <= paddleWidth) &&
        (ballY >= paddle1Y - paddleHeight / 2) &&
        (ballY <= paddle1Y + paddleHeight / 2)) {
        ballVelocityX = -ballVelocityX;  // Reverse the horizontal velocity
    }

    if ((ballX + ballSize / 2 >= screenWidth - paddleWidth) &&
        (ballY >= paddle2Y - paddleHeight / 2) &&
        (ballY <= paddle2Y + paddleHeight / 2)) {
        ballVelocityX = -ballVelocityX;  // Reverse the horizontal velocity
    }
}

// Function to draw the paddles and ball on the LCD
void draw_game() {
    clearScreen(COLOR_BLACK);  // Clear the screen

    // Draw paddles and ball
    fillRectangle(0, paddle1Y - paddleHeight / 2, paddleWidth, paddleHeight, COLOR_WHITE);  // Paddle 1
    fillRectangle(screenWidth - paddleWidth, paddle2Y - paddleHeight / 2, paddleWidth, paddleHeight, COLOR_WHITE);  // Paddle 2
    fillRectangle(ballX - ballSize / 2, ballY - ballSize / 2, ballSize, ballSize, COLOR_WHITE);  // Ball
}

// Main function
void main() {
    // Initialize the LCD
    lcd_init_setup();

    // Main game loop
    while (1) {
        // Update game state
        update_game_state();

        // Draw the game on the LCD
        draw_game();

        // Delay for a short period to control the game speed
        __delay_cycles(10000);
    }
}
