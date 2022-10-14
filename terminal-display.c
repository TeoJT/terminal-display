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

int xbgrToANSI(int xbgr) {
    //The rgb values are all stored in one int in the format:
    //xxxxxxxxbbbbbbbbggggggggrrrrrrrr
    //where x is unused (for now)
    //Get the red, green, and blue 8-bit values from the int
    int r = (xbgr & 0x000000FF);       //red
    int g = (xbgr & 0x0000FF00) >> 8;  //green
    int b = (xbgr & 0x00FF0000) >> 16; //blue

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

    return c;
} 

// Call this function to update the screen.
void terminalDisplayRender() {

    // because we're rendering essentially rendering two rows of pixels at the same time,
    // let's define two index variables, one for the even row, and one for the odd row.

    //Print the number and return to start of line
    //causing the number to stay in place on the screen
    setCursorPosition(0, 0);



    // The way we render pixels to the terminal is we print a half block character,
    // and set the foreground and background colours. This essentially means one character = 2 pixels
    // super cool stuff.
    // This means that for each character we print to the terminal, we actually render 2 rows of pixels at 
    // the same time, so we need to grab colours at x y position and x y+1 position to 
    for (int y = 0; y < t_d.height; y+=2) {
        for (int x = 0; x < t_d.width; x++) {
            
            int i1 = *(t_d.buffer + t_d.width*y + x);
            int i2 = *(t_d.buffer + t_d.width*y + x + t_d.width);
            
            int topCol    = xbgrToANSI(i1);
            int bottomCol = xbgrToANSI(i2);


            // Finally, print the pixel to the terminal!
            // The \e[38;5; thingie is just some stdout code to determine the colour of the pixel,
            // which c determines.

            // NOTE: \e[48;5; is the background colour, \e[38;5; is the foreground colour.]
            
            printf("\e[48;5;%dm\e[38;5;%dm▀", bottomCol, topCol);
        }

        

        //Newline so that we draw the next row of pixels
        if (t_d.isDisplayingMessage) {
            if (y != t_d.height-1) {
                printf("\n");
            }
        }
        else {
            printf("\n");
        }
    }

    // Force the terminal to update.
    fflush(stdout);
}



//Basic function to render a pixel to the buffer.
void pixel(int x, int y, int c) {
    if (x < 0 || x > t_d.width-1 || y < 0 || y > t_d.height-1) {
        return;
    }
    *(t_d.buffer + t_d.width*y + x) = c;
}

int color(int r, int g, int b) {
    return (r) | (g << 8) | (b << 16);
}

int toRed(int r) {
    return r & 0x000000FF;
}

int toGreen(int g) {
    return g & 0x0000FF00 >> 8;
}

int toBlue(int b) {
    return b & 0x00FF0000 >> 16;
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

    t_d.width  = t_d.terminalWidth;
    
    // we need to double the height since 1 half-block character character = two pixels.
    t_d.height = (t_d.terminalHeight-2)*2;

    // Length of the buffer, should be obvious.
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
