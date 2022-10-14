#include <stdio.h>
#include <unistd.h>
#include "../terminal-display.c"

int main() {
    terminalDisplayInit();

    int numBits = 30;
    int bitx[numBits];
    int bity[numBits];
    int bitr[numBits];
    int bitg[numBits];
    int bitb[numBits];

    for (int b = 0; b < numBits; b++) {
        // Random position between 0 and width + height of screen.
        bitx[b] = rand() % t_d.width;
        bity[b] = rand() % t_d.height;

        // Random colour.
        bitr[b] = rand() % 256;
        bitg[b] = rand() % 256;
        bitb[b] = rand() % 256;
    }

   for (int i = 0; i < 999; i) {
    //Print the number and return to start of line
    //causing the number to stay in place on the screen

    for (int b = 0; b < numBits; b++) {
        pixel(bitx[b], bity[b], color(0, 0, 0));
        bity[b]++;
        if (bity[b] > t_d.height) {
            bity[b] = 0;
        }
        pixel(bitx[b], bity[b], color(bitr[b], bitg[b], bitb[b]));
    }
    terminalDisplayRender();

    //small delay
    //usleep(16000);
    usleep(100000);
   }

   system("clear");
   
   return 0;
}
