#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


#define BTN_UP 32
#define BTN_DOWN 33


int menuSelection = 0;
const int MENU_ITEMS = 2;
bool gameStarted = false;


struct Paddle { int x, y, w, h; } paddle;
struct Ball { int x, y, dx, dy, size; } ball;
struct FlappyBird { int y, vy, pipeX, pipeGapY; } bird;


int score = 0;


void initPonk() {
    paddle = {0, SCREEN_HEIGHT/2 - 4, 2, 8};
    ball = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 1, 1, 2};
    score = 0;
}


void initFlappy() {
    bird.y = SCREEN_HEIGHT / 2;
    bird.vy = 0;
    bird.pipeX = SCREEN_WIDTH;
    bird.pipeGapY = random(4, SCREEN_HEIGHT - 14);
    score = 0;
}


void setup() {
    Wire.begin(21, 22);
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    initPonk();
    initFlappy();
}


void showMenu() {
    static bool upPressed = false;
    static bool downPressed = false;


    if (digitalRead(BTN_UP) == LOW && !upPressed) {
        menuSelection--;
        if (menuSelection < 0) { menuSelection = MENU_ITEMS - 1; }
        upPressed = true;
    }
    if (digitalRead(BTN_UP) == HIGH) { upPressed = false; }


    if (digitalRead(BTN_DOWN) == LOW && !downPressed) {
        menuSelection++;
        if (menuSelection >= MENU_ITEMS) { menuSelection = 0; }
        downPressed = true;
    }
    if (digitalRead(BTN_DOWN) == HIGH) { downPressed = false; }


    if (digitalRead(BTN_UP) == LOW && digitalRead(BTN_DOWN) == LOW) {
        gameStarted = true;
    }


    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);


    for (int i = 0; i < MENU_ITEMS; i++) {
        display.setCursor(20, 10 + i*10);
        if (i == menuSelection) { display.print("> "); }
        else { display.print("  "); }
        if (i == 0) { display.println("Ponk"); }
        if (i == 1) { display.println("Flappy"); }
    }


    display.display();
}


void waitForButton() {
    while (digitalRead(BTN_UP) == HIGH && digitalRead(BTN_DOWN) == HIGH) {
        delay(10);
    }
}


void showScore() {
    display.clearDisplay();
    display.setCursor(20, 10);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.print("Score: ");
    display.println(score);
    display.display();
    waitForButton();
}


void Ponk() {
    if (digitalRead(BTN_UP) == LOW) { if (paddle.y > 0) { paddle.y--; } }
    if (digitalRead(BTN_DOWN) == LOW) { if (paddle.y < SCREEN_HEIGHT - paddle.h) { paddle.y++; } }


    ball.x += ball.dx;
    ball.y += ball.dy;


    if (ball.y <= 0 || ball.y >= SCREEN_HEIGHT - ball.size) { ball.dy = -ball.dy; }


    if (ball.x <= paddle.x + paddle.w) {
        if (ball.y + ball.size >= paddle.y && ball.y <= paddle.y + paddle.h) {
            ball.dx = -ball.dx;
            ball.x = paddle.x + paddle.w;
            score++;
        }
    }


    if (ball.x >= SCREEN_WIDTH - ball.size) { ball.dx = -ball.dx; }
    if (ball.x < 0) {
        showScore();
        initPonk();
        return;
    }


    display.clearDisplay();
    display.fillRect(paddle.x, paddle.y, paddle.w, paddle.h, SSD1306_WHITE);
    display.fillRect(ball.x, ball.y, ball.size, ball.size, SSD1306_WHITE);
    display.setCursor(SCREEN_WIDTH - 30, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.print(score);
    display.display();
    delay(20);
}


void Flappy() {
    if (digitalRead(BTN_UP) == LOW) { bird.vy = -2; }


    bird.vy += 1;
    if (bird.vy > 1) { bird.vy = 1; }
    if (bird.vy < -2) { bird.vy = -2; }


    bird.y += bird.vy;
    if (bird.y < 0) { bird.y = 0; bird.vy = 0; }
    if (bird.y > SCREEN_HEIGHT - 2) {
        showScore();
        initFlappy();
        return;
    }


    bird.pipeX -= 2;
    if (bird.pipeX < -5) {
        bird.pipeX = SCREEN_WIDTH;
        bird.pipeGapY = random(4, SCREEN_HEIGHT - 14);
        score++;
    }


    display.clearDisplay();
    display.fillRect(4, bird.y, 2, 2, SSD1306_WHITE);
    display.fillRect(bird.pipeX, 0, 5, bird.pipeGapY, SSD1306_WHITE);
    display.fillRect(bird.pipeX, bird.pipeGapY + 10, 6, SCREEN_HEIGHT - bird.pipeGapY - 10, SSD1306_WHITE);
    display.setCursor(SCREEN_WIDTH - 30, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.print(score);


    if ((bird.pipeX < 6 && bird.pipeX + 6 > 4) && (bird.y < bird.pipeGapY || bird.y > bird.pipeGapY + 10)) {
        showScore();
        initFlappy();
        return;
    }


    display.display();
    delay(50);
}


void loop() {
    if (!gameStarted) { showMenu(); }
    else {
        if (menuSelection == 0) { Ponk(); }
        if (menuSelection == 1) { Flappy(); }
    }
}