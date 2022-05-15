# ncursesSnake
The game of snake implemented in C using ncurses.

Requires the ncurses library to be installed.
There's some code in here that uses sleep(). I used a macro to use a windows function if its compiled on windows and the unix sleep() otherwise.
I did not actually test it on windows though. It works on linux just fine though.
