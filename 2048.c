#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define GRID_SIZE 4 
#define CELL_HEIGHT 3 
#define CELL_WIDTH 7 

// structura pentru celule
typedef struct {
  int value; // valoarea din celula
} Cell;

// structura pentru joc
typedef struct {
  Cell board[GRID_SIZE][GRID_SIZE];
  int score; // scorul jocului
  bool gameOver; // indica daca jocul s-a terminat
  bool gameWon; // indica daca jocul a fost castigat
} GameState;

enum MenuOption { NEW_GAME, RESUME, QUIT, MENU_OPTIONS };

// declarare functii
void drawMenu(int highlight);
void initGame(GameState *gameState);
void drawGame(GameState *gameState);
void addRandomCell(GameState *gameState);
void drawControlPanel(GameState *gameState, int startx, int starty);
bool canMoveLeft(GameState *gameState);
void moveLeft(GameState *gameState);
bool canMoveRight(GameState *gameState);
void moveRight(GameState *gameState);
bool canMoveUp(GameState *gameState);
void moveUp(GameState *gameState);
bool canMoveDown(GameState *gameState);
void moveDown(GameState *gameState);

// functie pentru meniul jocului
void drawMenu(int highlight) { 
  char *choices[MENU_OPTIONS] = {"New Game", "Resume", "Quit"};
  clear();
  int i;
  for (i = 0; i < MENU_OPTIONS; ++i) {
    if (i == highlight) { // evidentiaza optiunea selectata 
      attron(A_REVERSE); // se inverseaza culorile
      mvprintw(2 + i, 1, "%s", choices[i]); // afiseaza varianta inversata
      attroff(A_REVERSE); // revine la culoarea initiala
    } else {
      mvprintw(2 + i, 1, "%s", choices[i]);
    }
  }
}

// functie initializare joc
void initGame(GameState *gameState) {
  gameState->score = 0; 
  gameState->gameOver = false; 
  gameState->gameWon = false;
  int i;
  // tabla de joc cu celule goale
  for (i = 0; i < GRID_SIZE; i++) {
    int j;
    for (j = 0; j < GRID_SIZE; j++) {
      gameState->board[i][j].value = 0; // fiecare celula are valoarea 0
    }
  }
  addRandomCell(gameState); // adaugarea unei celule pe tabla aleatoriu
  addRandomCell(gameState); // adaugarea unei alte celule pe tabla aleatoriu
}

// functie initializare culori
void initializeColors() {
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_BLUE, COLOR_BLUE);
}

// functie desenare tabla 
void drawBoard() {
  int gridWidth = 4;
  int gridHeight = 4;
  int cellWidth = 12;
  int cellHeight = 6;
  initializeColors();
  attron(COLOR_PAIR(2)); // activeaza perechea de culori
  int i;
  // parcurge fiecare celula pentru a o desena 
  for (i = 0; i < gridHeight; ++i) {
    int j;
    for (j = 0; j < gridWidth; ++j) {
      int startY = i * cellHeight;
      int startX = j * cellWidth;
      attron(COLOR_PAIR(2));
      int y;
      // fiecare celula este desenata ca un patrat
      for (y = startY; y < startY + cellHeight; ++y) {
        int x;
        for (x = startX; x < startX + cellWidth; ++x) {
          if (x == startX || y == startY || x == startX + cellWidth - 1 ||
              y == startY + cellHeight - 1) {
            mvaddch(y, x, ' '); // spatiu pentru marginea celulei
          }
        }
      }
      attroff(COLOR_PAIR(2));
    }
  }
}

// functie desenare joc
void drawGame(GameState *gameState) {
  clear();
  int i;
  // parcurge tabla de joc si deseneaza celulele
  for (i = 0; i < GRID_SIZE; i++) {
    int j;
    for (j = 0; j < GRID_SIZE; j++) {
      int cellX = i * (CELL_HEIGHT + 1);
      int cellY = j * (CELL_WIDTH + 1);
      attron(COLOR_PAIR(1));
      int y;
      for (y = 0; y < CELL_HEIGHT; y++) {
        int x;
        for (x = 0; x < CELL_WIDTH; x++) {
          mvaddch(cellX + y, cellY + x, ' ');
        }
      }
      attroff(COLOR_PAIR(1));
      if (gameState->board[i][j].value != 0) {
        int digitX = cellY + (CELL_WIDTH - 1) / 2;
        int digitY = cellX + CELL_HEIGHT / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(digitY, digitX, "%d", gameState->board[i][j].value);
        attroff(COLOR_PAIR(1) | A_BOLD);
      }
    }
  }
  int panel_startX = (GRID_SIZE * (CELL_WIDTH + 1)) + 2;
  int panel_startY = 0;
  drawControlPanel(gameState, panel_startX, panel_startY);
  // afiseaza mesaje aferente starii jocului
  if (gameState->gameWon) {
    mvprintw(panel_startY + 2 + GRID_SIZE, panel_startX,
             "Congratulations! You won!");
  } else if (gameState->gameOver) {
    mvprintw(panel_startY + 2 + GRID_SIZE, panel_startX,
             "Game Over! Press 'r' to restart or 'q' to quit.");
  }
  refresh(); 
}

// functie adaugare celula aleatorie 
void addRandomCell(GameState *gameState) {
  int x, y;
  // gasire loc liber pe tabla
  do {
    x = rand() % GRID_SIZE;
    y = rand() % GRID_SIZE;
  } while (gameState->board[x][y].value != 0);
  int val = (rand() % 2 + 1) * 2; // generare valoare (2 sau 4)
  gameState->board[x][y].value = val;
}

// functie pentru panoul de control
void drawControlPanel(GameState *gameState, int startX, int startY) {
  time_t now;
  struct tm *timeinfo;
  char timeString[64];
  time(&now); 
  timeinfo = localtime(&now);
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeinfo);
  mvprintw(startY, startX, "Score: %d", gameState->score);
  mvprintw(startY + 1, startX, "Time: %s", timeString);
  mvprintw(startY + 2, startX, "Commands: [Arrows] Move [Q] Quit");
}

// verificare disponibilitate mutare spre stanga
bool canMoveLeft(GameState *gameState) {
  int i;
  // se parcurge fiecare rand si coloana
  for (i = 0; i < GRID_SIZE; i++) {
    int j;
    for (j = 1; j < GRID_SIZE; j++) {
      if (gameState->board[i][j].value != 0 &&
          (gameState->board[i][j - 1].value == 0 ||
           gameState->board[i][j - 1].value == gameState->board[i][j].value)) {
        return true;
      }
    }
  }
  return false;
}

// functie mutare celule la stanga
void moveLeft(GameState *gameState) {
  int i;
  for (i = 0; i < GRID_SIZE; i++) {
    int merged[GRID_SIZE] = {0}; // celule combinate
    int j;
    for (j = 1; j < GRID_SIZE; j++) {
      if (gameState->board[i][j].value != 0) {
        int k = j;
        // mutarea si combinarea, daca este necesar, a celulelor 
        while (k > 0 && (gameState->board[i][k - 1].value == 0 ||
                         (gameState->board[i][k - 1].value ==
                              gameState->board[i][k].value &&
                          merged[k - 1] == 0))) {
          if (gameState->board[i][k - 1].value == 0) {
            gameState->board[i][k - 1].value = gameState->board[i][k].value;
            gameState->board[i][k].value = 0;
          } else if (gameState->board[i][k - 1].value ==
                     gameState->board[i][k].value) {
            gameState->board[i][k - 1].value *= 2;
            gameState->board[i][k].value = 0;
            gameState->score += gameState->board[i][k - 1].value;
            merged[k - 1] = 1;
          }
          k--;
        }
      }
    }
  }
}

// verificare disponibilitate mutare spre dreapta 
bool canMoveRight(GameState *gameState) {
  int i;
  // se parcurge fiecare rand si coloana
  for (i = 0; i < GRID_SIZE; i++) {
    int j;
    for (j = 0; j < GRID_SIZE - 1; j++) {
      if (gameState->board[i][j].value != 0 &&
          (gameState->board[i][j + 1].value == 0 ||
           gameState->board[i][j + 1].value == gameState->board[i][j].value)) {
        return true;
      }
    }
  }
  return false;
}

// functie mutare celule la dreapta
void moveRight(GameState *gameState) {
  int i;
  for (i = 0; i < GRID_SIZE; i++) {
    int merged[GRID_SIZE] = {0}; // celule combinate
    int j;
    for (j = GRID_SIZE - 2; j >= 0; j--) {
      if (gameState->board[i][j].value != 0) {
        int k = j;
        // mutarea si combinarea, daca este necesar, a celulelor 
        while (k < GRID_SIZE - 1 && (gameState->board[i][k + 1].value == 0 ||
                                     (gameState->board[i][k + 1].value ==
                                          gameState->board[i][k].value &&
                                      merged[k + 1] == 0))) {
          if (gameState->board[i][k + 1].value == 0) {
            gameState->board[i][k + 1].value = gameState->board[i][k].value;
            gameState->board[i][k].value = 0;
          } else if (gameState->board[i][k + 1].value ==
                     gameState->board[i][k].value) {
            gameState->board[i][k + 1].value *= 2;
            gameState->board[i][k].value = 0;
            gameState->score += gameState->board[i][k + 1].value;
            merged[k + 1] = 1;
          }
          k++;
        }
      }
    }
  }
}

// verificare disponibilitate mutare sus
bool canMoveUp(GameState *gameState) {
  int i;
  // se parcurge fiecare rand si coloana
  for (i = 1; i < GRID_SIZE; i++) {
    int j;
    for (j = 0; j < GRID_SIZE; j++) {
      if (gameState->board[i][j].value != 0 &&
          (gameState->board[i - 1][j].value == 0 ||
           gameState->board[i - 1][j].value == gameState->board[i][j].value)) {
        return true;
      }
    }
  }
  return false;
}

// functie mutare celule sus
void moveUp(GameState *gameState) {
  int j;
  for (j = 0; j < GRID_SIZE; j++) {
    int merged[GRID_SIZE] = {0}; // celule combinate
    int i;
    for (i = 1; i < GRID_SIZE; i++) {
      if (gameState->board[i][j].value != 0) {
        int k = i;
        // mutarea si combinarea, daca este necesar, a celulelor
        while (k > 0 && (gameState->board[k - 1][j].value == 0 ||
                         (gameState->board[k - 1][j].value ==
                              gameState->board[k][j].value &&
                          merged[k - 1] == 0))) {
          if (gameState->board[k - 1][j].value == 0) {
            gameState->board[k - 1][j].value = gameState->board[k][j].value;
            gameState->board[k][j].value = 0;
          } else if (gameState->board[k - 1][j].value ==
                     gameState->board[k][j].value) {
            gameState->board[k - 1][j].value *= 2;
            gameState->board[k][j].value = 0;
            gameState->score += gameState->board[k - 1][j].value;
            merged[k - 1] = 1;
          }
          k--;
        }
      }
    }
  }
}

// verificare disponibilitate mutare jos
bool canMoveDown(GameState *gameState) {
  int j;
  // se parcurge fiecare rand si coloana
  for (j = 0; j < GRID_SIZE; j++) {
    int i;
    for (i = 0; i < GRID_SIZE - 1; i++) {
      if (gameState->board[i][j].value != 0 &&
          (gameState->board[i + 1][j].value == 0 ||
           gameState->board[i + 1][j].value == gameState->board[i][j].value)) {
        return true;
      }
    }
  }
  return false;
}

// functie mutare celule jos
void moveDown(GameState *gameState) {
  int j;
  for (j = 0; j < GRID_SIZE; j++) {
    int merged[GRID_SIZE] = {0}; // celule combinate
    int i;
    for (i = GRID_SIZE - 2; i >= 0; i--) {
      if (gameState->board[i][j].value != 0) {
        int k = i;
        // mutarea si combinarea, daca este necesar, a celulelor
        while (k < GRID_SIZE - 1 && (gameState->board[k + 1][j].value == 0 ||
                                     (gameState->board[k + 1][j].value ==
                                          gameState->board[k][j].value &&
                                      merged[k + 1] == 0))) {
          if (gameState->board[k + 1][j].value == 0) {
            gameState->board[k + 1][j].value = gameState->board[k][j].value;
            gameState->board[k][j].value = 0;
          } else if (gameState->board[k + 1][j].value ==
                     gameState->board[k][j].value) {
            gameState->board[k + 1][j].value *= 2;
            gameState->board[k][j].value = 0;
            gameState->score += gameState->board[k + 1][j].value;
            merged[k + 1] = 1;
          }
          k++;
        }
      }
    }
  }
}

// verificare daca a fost castigat jocul
bool isGameWon(GameState *gameState) {
  int i;
  for (i = 0; i < GRID_SIZE; i++) {
    int j;
    for (j = 0; j < GRID_SIZE; j++) {
      if (gameState->board[i][j].value == 2048) {
        return true;
      }
    }
  }
  return false;
}

// verificare daca a fost pierdut jocul
bool isGameLost(GameState *gameState) {
  int i;
  for (i = 0; i < GRID_SIZE; i++) {
    int j;
    for (j = 0; j < GRID_SIZE; j++) {
      if (gameState->board[i][j].value == 0) {
        return false;
      }
    }
  }
  // dacă nu există celule goale, verifică dacă există mișcări posibile 
  if (canMoveLeft(gameState) || canMoveRight(gameState) ||
      canMoveUp(gameState) || canMoveDown(gameState)) {
    return false; // exista miscari posibile 
  }
  return true; // nu exista miscari posibile
}

int main() {
  initscr();             
  cbreak();              
  noecho();              
  keypad(stdscr, TRUE); // activare citire taste speciale
  start_color();        
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  init_pair(2, COLOR_BLACK, COLOR_WHITE);
  bkgd(COLOR_PAIR(2));
  srand(time(NULL));  

  GameState gameState;
  int highlight = 0;  
  bool running = true; 
  while (1) {
    drawMenu(highlight);
    refresh();
    int c = getch(); // citire intrare de la tastatura 
    // gestionare navigare meniu
    switch (c) {
      case KEY_UP:
        if (highlight > 0) highlight--; // mers in sus in meniu
        break;
      case KEY_DOWN:
        if (highlight < MENU_OPTIONS - 1) { // mers in jos in meniu
          highlight++;
        } else {
          highlight = NEW_GAME; 
        }
        break;
      case 10: // enter
        int choice = highlight;
        if (choice == QUIT) {
          endwin(); // inchide jocul
          return 0;
        } else if (choice == NEW_GAME) {
          initGame(&gameState); // initializare joc nou
          clear();
          drawGame(&gameState);
          refresh();
          while (1) {
            if (!gameState.gameOver) {
              switch (c) {
                case 'r':
                case 'R':
                  initGame(&gameState);
                  gameState.gameOver = false;
                  clear();
                  drawGame(&gameState);
                  refresh();
                  break;
              }
            }
            int move = getch(); // citeste urmatoarea miscare
            if (move == 'q' || move == 'Q') {
              break;
            } else if (move == KEY_LEFT && canMoveLeft(&gameState)) {
              moveLeft(&gameState); // executare miscare la stanga
              addRandomCell(&gameState);
              clear();
              drawGame(&gameState);
              refresh();
            } else if (move == KEY_RIGHT && canMoveRight(&gameState)) {
              moveRight(&gameState); // executare miscare la dreapta
              addRandomCell(&gameState);
              clear();
              drawGame(&gameState);
              refresh();
            } else if (move == KEY_UP && canMoveUp(&gameState)) {
              moveUp(&gameState); // executare miscare sus
              addRandomCell(&gameState);
              clear();
              drawGame(&gameState);
              refresh();
            } else if (move == KEY_DOWN && canMoveDown(&gameState)) {
              moveDown(&gameState); // executare miscare jos
              addRandomCell(&gameState);
              clear();
              drawGame(&gameState);
              refresh();
            }
            // verificare daca a fost castigat jocul
            if (isGameWon(&gameState)) {
              mvprintw(GRID_SIZE + 2, 33, " Congratulations! You won!");
              refresh();
              break;
            }
            // verificare daca a fost pierdut jocul
            if (isGameLost(&gameState)) {
              mvprintw(GRID_SIZE + 2, 33,
                       " Game Over! Press 'r' to restart or 'q' to quit.");
              refresh();
              int gameOverCh;
              do {
                gameOverCh = getch();
              } while (gameOverCh != 'q' && gameOverCh != 'r');
              if (gameOverCh == 'r') {
                initGame(&gameState); // reinitializare joc
                gameState.gameOver = false;
                clear();
                drawGame(&gameState);
                refresh();
                continue;
              } else if (gameOverCh == 'q') {
                break;
              }
            }
          }
        } else if (choice == RESUME) {
          break;
        }
    }
  }
  if (isGameWon(&gameState)) {
    gameState.gameWon = true;  // setează starea de câștig
    refresh();
  }
  if (isGameLost(&gameState)) {
    gameState.gameOver = true;  // setează starea de pierdere
    refresh();
    int gameOverCh;
    do {
      gameOverCh = getch();
    } while (gameOverCh != 'q' && gameOverCh != 'r');
    if (gameOverCh == 'r') {
      initGame(&gameState);
      clear();
      drawGame(&gameState);
      gameState.gameOver = false;
    }
  }

  endwin();
  return 0;
}