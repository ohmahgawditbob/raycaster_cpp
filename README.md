# raycaster_cpp
A C++ Raycaster made from scratch

Graphics Library: olcPixelGameEngine
https://github.com/OneLoneCoder/olcPixelGameEngine

I just use it to display stuff on the screen.

## Compiling
I like to use my skeleton-key g++ command:
`g++ -pthread main.cpp -lGL -lpng -lX11 -lstdc++ -g -o rend_engine.o`

Be sure you include the directory for the olcPixelGameEngine if you haven't moved it to your global include area!
I have mine installed in `/usr/include/olc` for convenience.

I didn't include the olcPixelGameEngine because i'm not too experienced in all of the legal stuff that surrounds open source licenses yet, so i'd rather not mess with it and simply give the link to download.
