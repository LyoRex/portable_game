#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include <stdint.h>

#define TFT_DC P3_6
#define TFT_CS P3_5
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

uint16_t sWidth = tft.width();
uint16_t sHeight = tft.height();
int16_t textCursorX = 0;
int16_t textCursorY = 0;

/*  STATES
 *    0: start menu
 *    1: rules menu
 *    2: game screen
 */
uint16_t gameState = -1;
uint8_t selection = -1;

/*  BUTTONS
 *    35: DOWN
 *    36: LEFT
 *    37: UP
 *    38: RIGHT
 *    39: SELECT
 *    40: RETURN
 */
const int NUM_BUTTONS = 6;

int buttonPins[NUM_BUTTONS] = {35,36,37,38,39,40};      // Pin numbers of buttons being used
int curButtonStates[NUM_BUTTONS] = {0};                 // Current state of push buttons
int prevButtonStates[NUM_BUTTONS] = {0};                // State of push buttons in previous frame
unsigned char buttonPressed[NUM_BUTTONS] = {0};         // Indicates if button was just pressed down this frame
unsigned char buttonReleased[NUM_BUTTONS] = {0};        // Indicates if button was just released this frame

int winStreak = 0;
int longestWinStreak = 0;

/*
 * Check states of buttons
 */
void readButtons() {
  for(int i = 0; i < NUM_BUTTONS; i++) {
    curButtonStates[i] = digitalRead(buttonPins[i]);
    if(curButtonStates[i] != prevButtonStates[i]) {   // Run only if current state differs from previous frame
      if(curButtonStates[i] == 1) {                   // Button is pressed if button is currently down
        buttonPressed[i] = 1;
        buttonReleased[i] = 0;
      }
      else if(curButtonStates[i] == 0) {              // Button is released if button is currently up
        buttonReleased[i] = 1;
        buttonPressed[i] = 0;
      }
    }
    else {
      buttonPressed[i] = 0;       // Button is not pressed if current state = previous state
      buttonReleased[i] = 0;      // Button is not released if current state = previous state
    }
    prevButtonStates[i] = curButtonStates[i];
  }
}

void setup() {
  // Set pin mode of all buttons
  for(int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  
  Serial.begin(9600);
  randomSeed(analogRead(0));
  tft.begin();

  tft.setRotation(3);
  tft.setTextWrap(false);
  sWidth = tft.width();
  sHeight = tft.height();

  switchState(0);
}

void loop(void) {
  readButtons();
  switch(gameState) {
    case 0:
      start_menu();
      break;
    case 1:
      rules_menu();
      break;
    case 2:
      game_menu();
      break;
  }
}

void start_menu() {
  if(buttonReleased[4]){ 
    Serial.println("Going to game menu");
    switchState(2);
  }
  if(buttonReleased[5]){ 
    Serial.println("Going to rules menu");
    switchState(1);
  }
}

void rules_menu() {
  if(buttonReleased[5]){ 
    Serial.println("Going to start menu");
    switchState(0);
  }
}


void game_menu() {
  if (buttonReleased[0] || buttonReleased[2]) {
    if (buttonReleased[0]) {
      selection = (selection + 1) % 3;
    }
    else if (buttonReleased[2]) {
      selection = (selection + 2) % 3;
    }
    tft.drawRoundRect(30, 45, 50, 50, 5, ILI9341_BLACK);
    tft.drawRoundRect(30, 105, 50, 50, 5, ILI9341_BLACK);
    tft.drawRoundRect(30, 165, 50, 50, 5, ILI9341_BLACK);
    if (selection == 0) {
      tft.drawRoundRect(30, 45, 50, 50, 5, ILI9341_RED);
    }
    else if (selection == 1) {
      tft.drawRoundRect(30, 105, 50, 50, 5, ILI9341_RED);
    }
    else if (selection == 2) {
      tft.drawRoundRect(30, 165, 50, 50, 5, ILI9341_RED);
    }
  }
  else if(buttonReleased[4]) {
    textCursorX = sWidth/2;
    textCursorY = sHeight/2;
    int cpuChoice = random(3);
    switch(cpuChoice) {
      case 0:
        tft.drawRoundRect(sWidth-80, 45, 50, 50, 5, ILI9341_RED);
        break;
      case 1:
        tft.drawRoundRect(sWidth-80, 105, 50, 50, 5, ILI9341_RED);
        break;
      case 2:
        tft.drawRoundRect(sWidth-80, 165, 50, 50, 5, ILI9341_RED);
        break;
    }
    if (selection == 0) {
      if(cpuChoice == 0) {
        printCentered("It's a tie...");
        winStreak = 0;
      }
      else if(cpuChoice == 1) {
        printCentered("You lose!");
        winStreak = 0;
      }
      else if(cpuChoice == 2) {
        printCentered("You win!");
        winStreak += 1;
      }
    }
    else if(selection == 1) {
      if(cpuChoice == 0) {
        printCentered("You win!");
        winStreak += 1;
      }
      else if(cpuChoice == 1) {
        printCentered("It's a tie...");
        winStreak = 0;
      }
      else if(cpuChoice == 2) {
        printCentered("You lose!");
        winStreak = 0;
      }
    }
    else if(selection == 2) {
      if(cpuChoice == 0) {
        printCentered("You lose!");
        winStreak = 0;
      }
      else if(cpuChoice == 1) {
        printCentered("You win!");
        winStreak += 1;
      }
      else if(cpuChoice == 2) {
        printCentered("It's a tie...");
        winStreak = 0;
      }
    }
    if(winStreak > longestWinStreak) {
      longestWinStreak = winStreak;
    }
    delay(3000);
    initializeState2();
  }
  else if(buttonReleased[5]) {
    switchState(0);
  }
}

void switchState(uint16_t newState) {
  gameState = newState;
  switch(newState) {
    case 0:
      initializeState0();
      break;
    case 1:
      initializeState1();
      break;
    case 2:
      initializeState2();
      break;
  }
}

// HOME SCREEN
void initializeState0() {
  tft.fillScreen(ILI9341_RED);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(3);
  textCursorX = sWidth/2;
  textCursorY = sHeight/4;
  printCentered("Welcome to");
  printCentered("the Game");
  printCentered("---------");
  tft.setTextSize(2);
  printEmptyLine();
  printCentered("\tSTART: Play");
  printEmptyLine();
  printEmptyLine();
  printEmptyLine();
  printCentered("\tRETURN: Rules");
}

// RULES SCREEN
void initializeState1() {
  tft.fillScreen(ILI9341_GREEN);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  textCursorX = sWidth/2;
  textCursorY = sHeight/8;
  printCentered("Rules");
  printCentered("---------");
  tft.setTextSize(1);
  tft.setTextWrap(true);
  tft.setCursor(0, textCursorY);
  tft.println("-This game is Rock-Paper-Scissors");
  tft.println("-You will be playing against a CPU");
  tft.println("-Use the UP and DOWN buttons to choose");
  tft.println(" and press START to select your choice");
  tft.println("   -Rock loses to paper");
  tft.println("   -Paper loses to scissors");
  tft.println("   -Scissors loses to rock");
  tft.println("-To quit, press the RETURN button");
  tft.println();
  tft.println();
  tft.println();
  tft.println("Longest win streak: " + String(longestWinStreak));
  tft.setTextWrap(false);
}

void drawRock(int x, int y) {
  tft.fillRoundRect(x, y, 40, 40, 11, 0x7BEF);
}

void drawPaper(int x, int y) {
  tft.drawLine(x, y+25, x+25, y, ILI9341_WHITE);
  tft.drawLine(x, y+25, x+15, y+40, ILI9341_WHITE);
  tft.drawLine(x+15, y+40, x+40, y+15, ILI9341_WHITE);
  tft.drawLine(x+25, y, x+40, y+15, ILI9341_WHITE);
}
  
void drawScissors(int x, int y) {
  tft.drawCircle(x+6, y+22, 6, ILI9341_RED);
  tft.drawLine(x+12, y+22, x+40, y+17, ILI9341_WHITE);
  tft.drawCircle(x+18, y+34, 6, ILI9341_RED);
  tft.drawLine(x+18, y+28, x+23, y, ILI9341_WHITE);
}

// PLAY SCREEN
void initializeState2() {
  tft.fillScreen(ILI9341_BLACK);

  // DRAW OPTIONS FOR PLAYER
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(10, 65);
  tft.setTextSize(2);
  tft.print("A");
  drawRock(35, 50);
  tft.setCursor(10, 125);
  tft.setTextSize(2);
  tft.print("B");
  drawPaper(35, 110);
  tft.setCursor(10, 185);
  tft.setTextSize(2);
  tft.print("C");
  drawScissors(35, 170);
  tft.setCursor(30, 25);
  tft.setTextSize(2);
  tft.print("YOU");
  
  // DRAW OPTIONS FOR CPU
  drawRock(sWidth - 75, 50);
  drawPaper(sWidth - 75, 110);
  drawScissors(sWidth - 75, 170);
  tft.setCursor(sWidth - 80, 25);
  tft.setTextSize(2);
  tft.print("CPU");

  tft.setTextSize(1);
  textCursorX = sWidth/2;
  textCursorY = sHeight/10;
  printCentered("Select an option");
  printCentered("(A, B, C)");
  textCursorY = 9 * sHeight/10;

  String winStreakText = "Win Streak: " + String(winStreak);
  char winStreakBuf[winStreakText.length() + 1];
  winStreakText.toCharArray(winStreakBuf, winStreakText.length() + 1);
  Serial.println(winStreakBuf);
  printCentered(winStreakBuf);

  selection = 0;
  tft.drawRoundRect(30, 45, 50, 50, 5, ILI9341_RED);
}

void printCentered(char* text) {
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  tft.getTextBounds(text, textCursorX, textCursorY, &x1, &y1, &width, &height);

  // display on horizontal and vertical center
  tft.setCursor((textCursorX - width / 2), (textCursorY - height / 2));
  tft.println(text); // text to display
  textCursorY = y1 + height;
}

void printEmptyLine() {
  tft.println();
  textCursorY = tft.getCursorY();
}
