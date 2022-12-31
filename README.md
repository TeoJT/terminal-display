# terminal-display
A really basic way to display pixel graphics in your terminal.

## Features
- 2 demos!
- Play snaek!
- Draw pixels directly to the terminal!
- Uses half-block characters so every terminal character is worth 2 pixels!
- A whopping 256 colours!
- Easy-to-use functions!

## Usage
- `terminal-display.c` contains all the code you need to set up a basic display. Demo's are not required.
- Run `terminalDisplayInit()` upon starting to begin using the terminal display!
- Width and height will be set automatically based on the size of the terminal.
- `t_d.width` and `t_d.height` to get width and height in pixels.
- `pixel(x, y, color(r, g, b))` to place a pixel anywhere in the terminal.
- `color(r, g, b)` simply translate rgb values into a single int that holds the ~~colour~~ color data of the pixel. I call it the xbgr int.
- `getPixel(x, y)` returns the colour of the pixel stored in an xbgr int.
- To get individual rgb values from an xbgr int, pass the value through `toRed(c)`, `toGreen(c)` and `toBlue(c)`.

Check out the demo's for some neat examples on how to use em.

## Todo
- Create a demo that actually uses `toRed(c)`, `toGreen(c)` and `toBlue(c)`.
- Add some useful functions like `line`, `triangle`, and more shape-drawing functions.
- Create some external terminal output for stdout from printf since the terminal is obviously being used for displaying pixels

