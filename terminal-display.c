#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


struct TerminalDisplay {
    int terminalWidth;
    int terminalHeight;
    int width;
    int height;
    int length;
    int isDisplayingMessage;
    char* dbMssg;
    int* buffer;
} t_d;

void setCursorPosition(int XPos, int YPos) {
    printf("\033[%d;%dH",YPos+1,XPos+1);
}
void getCursor(int* x, int* y) {
   printf("\033[6n");  /* This escape sequence !writes! the current
                          coordinates to the terminal.
                          We then have to read it from there, see [4,5].
                          Needs <termios.h>,<unistd.h> and some others */
   scanf("\033[%d;%dR", x, y);
}

// Call this function to update the screen.
void terminalDisplayRender() {
    int* i = t_d.buffer;

    //Print the number and return to start of line
    //causing the number to stay in place on the screen
    setCursorPosition(0, 0);


    for (int y = 0; y < t_d.height; y++) {
        for (int x = 0; x < t_d.width; x++) {

            //The rgb values are all stored in one int in the format:
            //xxxxxxxxbbbbbbbbggggggggrrrrrrrr
            //where x is unused (for now)
            //Get the red, green, and blue 8-bit values from the int
            int r = (*i & 0x000000FF);       //red
            int g = (*i & 0x0000FF00) >> 8;  //green
            int b = (*i & 0x00FF0000) >> 16; //blue

            // In case you're wondering:
            // "why don't we store r, g and b as their own bytes instead of
            // storing it in an int? Wouldn't that be more memory efficient
            // since we're using 24 bits instead of 32 bits?"
            //
            // A 32-bit memory read (with all rgb values) can be completed in 
            // one cycle while reading the individual bytes r, g, and then b
            // needs 3 memory reads. The buffer isn't typically big anyways, 
            // so it doesn't matter if we're wasting a byte for every pixel.
            // Plus we could always use that byte later on for displaying maybe
            // a character instead of text.


            // Each colour in the terminal only has 6 different brightness values,
            // so we gotta divide to get a brightness value from 0-255 to 0-5
            float brightnessFactor = 256/6;
            int rr = (int)((r-4)/brightnessFactor);
            int gg = (int)((g-4)/brightnessFactor);
            int bb = (int)((b-4)/brightnessFactor);

            // When gradually going from colour values 0-255, blue gets brighter until
            // it goes back to 0 and then green increases a brightness level, and then
            // when green reaches maximum brightness and resets to 0, red increases a 
            // brightness level.
            // Remember, r g and b only have 6 brightness values.
            int c = 16 + bb + gg*6 + rr*6*6;

            // Go to the next pixel in the buffer.
            i += 1;

            // Finally, print the pixel to the terminal!
            printf("\e[38;5;%dm██", c);
        }

        

        //Newline so that we draw the next row of pixels
        if (y == t_d.height-1 && t_d.isDisplayingMessage) {
            printf("\n");
        }
    }

    // Force the terminal to update.
    fflush(stdout);
}

//Basic function to render a pixel to the buffer.
//Although it has a f*ck ton of arguments lmao.
void pixel(int x, int y, int r, int g, int b) {
    *(t_d.buffer + t_d.width*y + x) = (r) | (g << 8) | (b << 16);
}

// Returns an int containing the colours in the following format:
// xxxxxxxxbbbbbbbbggggggggrrrrrrrr
int getPixel(int x, int y) {
    return *(t_d.buffer + t_d.width*y + x);
}

// Fills the whole buffer with the specified colour
void fill(int r, int g, int b) {
    int l = t_d.width*t_d.height;
    for (int i = 0; i < l; i++) {
        *(t_d.buffer + i) = (r) | (g << 8) | (b << 16);
    }
}

int* terminalDisplayInit() {
    // Just a lil hello message even though its probably not visible
    printf("terminal-display\n");

    // Get the width and height of the terminal
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // Set properties.
    t_d.terminalWidth  = w.ws_col;
    t_d.terminalHeight = w.ws_row;
    t_d.width  = t_d.terminalWidth/2;   // divided by two because two block characters makes up a pixel.
    t_d.height = t_d.terminalHeight-2;
    t_d.length = t_d.width*t_d.height;
    t_d.isDisplayingMessage = 0;

    // Allocate memory to the buffer.
    t_d.buffer = (int*) malloc(t_d.length*sizeof(int));

    // Clear the screen and render a red, green, and blue pixel to test if it works.
    fill(0,0,0);
    // Note: it won't actually render a rgb pixel lmao.
    //*(t_d.buffer)   = 0x000000FF;
    //*(t_d.buffer)   = 0x00FFFF00;
    //*(t_d.buffer)   = 0x00FF0000;

    // Call the clear command in Linux so that we have a lovely clean screen
    // to render on.
    system("clear");
}
