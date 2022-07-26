
#include "../terminal-display.c"
#include <termios.h>
#include <fcntl.h>
#include <termios.h>

#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4

// Returns 1 if a keyboard press has been detected.
int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

// Get the key that has been pressed.
char getch()
{
    char c;
    system("stty raw");
    c= getchar();
    system("stty sane");
    return(c);
}

// Generates an apple.
// Returns the pixel memory address of the apple.
int* generateApple() {
    // 1. Generate a random position
    // 2. If the random position is filled, move right in the buffer
    //    until a position is found.
    int pos = (rand() % t_d.length);
    printf("%d\n", pos);
    while (*(t_d.buffer + pos) != 0x00000000) {
        pos++;
        if (pos > t_d.length) {
            pos = 0;
        }
    }
    return (t_d.buffer + pos);
}

void D(int x, int y) {
    pixel(0+x, 0+y, 255,255,255);
    pixel(1+x, 0+y, 255,255,255);
    pixel(2+x, 1+y, 255,255,255);
    pixel(2+x, 2+y, 255,255,255);
    pixel(2+x, 3+y, 255,255,255);
    pixel(0+x, 1+y, 255,255,255);
    pixel(0+x, 2+y, 255,255,255);
    pixel(0+x, 3+y, 255,255,255);
    pixel(0+x, 4+y, 255,255,255);
    pixel(1+x, 4+y, 255,255,255);
}

void E(int x, int y) {
    pixel(0+x, 0+y, 255,255,255);
    pixel(0+x, 1+y, 255,255,255);
    pixel(0+x, 2+y, 255,255,255);
    pixel(0+x, 3+y, 255,255,255);
    pixel(0+x, 4+y, 255,255,255);
    pixel(1+x, 0+y, 255,255,255);
    pixel(2+x, 0+y, 255,255,255);
    pixel(1+x, 2+y, 255,255,255);
    pixel(1+x, 4+y, 255,255,255);
    pixel(2+x, 4+y, 255,255,255);
    
}

void main() {
    terminalDisplayInit();

    // Snake's direction
    int dir = RIGHT;

    // Initialise the x and y pos to the middle of the screen.
    int hx = t_d.width/2 + 1, hy = t_d.height/2;
    int tx = t_d.width/2 - 1, ty = t_d.height/2;

    int  snakeBuffLength = t_d.length*sizeof(int);
    int* snakeBuff = (int*) malloc(snakeBuffLength);
    int* snakeBuffTail = snakeBuff;
    int  snakeLength = 2;

    *(snakeBuff + 0) = RIGHT;
    *(snakeBuff + 1) = RIGHT;
    *(snakeBuff + 2) = RIGHT;
    *(snakeBuff + 3) = RIGHT;

    int* applePos = generateApple();

    int alive = 1;

    // Just gradually increases.
    double t = 0;

    // Game loop
    while (alive) {
        if (kbhit()) {
            char key = getch();
            if ((key == 'W' || key == 'w') && dir != DOWN) {
                dir = UP;
            }
            if ((key == 'A' || key == 'a') && dir != RIGHT) {
                dir = LEFT;
            }
            if ((key == 'S' || key == 's') && dir != UP) {
                dir = DOWN;
            }
            if ((key == 'D' || key == 'd') && dir != LEFT) {
                dir = RIGHT;
            }
        }

        // Tail
        // Directly access the buffer to set the pixel black using the
        // position from the tail buffer.
        *(t_d.buffer + *snakeBuffTail) = 0x00000000;

        // Debug to show the snake's tail buffer.
        /*
        t_d.isDisplayingMessage = 1;
        for (int i = 0; i < 10; i++) {
            if (snakeBuffTail == (snakeBuff+i)) {
                printf("\033[41m");
            }
            printf("%d \033[0m", *(snakeBuff+i));
        }
        */

        *snakeBuffTail = hy * t_d.width + hx;
        

        snakeBuffTail++;
        if (snakeBuffTail > snakeBuff+snakeLength-1) {
            snakeBuffTail = snakeBuff;
        }


        // Head

        switch (dir) {
            case UP:
            hy--;
            break;
            case DOWN:
            hy++;
            break;
            case LEFT:
            hx--;
            break;
            case RIGHT:
            hx++;
            break;
        }

        // New position, detect what's in front of the snake.

        // Eat apple.
        if ((getPixel(hx, hy) & 0x000000FF) > 0) {
            *(snakeBuff+snakeLength) = *(snakeBuff+snakeLength-1);
            snakeLength++;
            applePos = generateApple();
        }
        if (hx > t_d.width-1 || hx < 0 || hy > t_d.height-1 || hy < 0) {
            // Dead
            alive = 0;
        }
        if (getPixel(hx, hy) == 0x0000FF00) {
            // Dead
            alive = 0;
        }

        pixel(hx, hy, 0, 255, 0);

        // Applepos is a pointer that points to the pixel of the apple.
        // Use it directly to render the apple.
        // Because red is the least-significant bit, we can just directly
        // write to the word without doing any fancy-ass bitwise operations.

        // Do a fancy sin operation to make it glow oooooooo
        *applePos = (int)(sin(t)*100+100+50);
        t += 0.5;



        terminalDisplayRender();
        



        usleep(100000);
    }
    
    D(t_d.width/2 - 4, t_d.height/2-2);
    E(t_d.width/2,     t_d.height/2-2);
    D(t_d.width/2 + 4, t_d.height/2-2);
    terminalDisplayRender();
    while (!kbhit()) {
        usleep(10000);
    }
    return 0;
}