Asterad, an Asteroids clone for Windows developed in C. Rendered with OpenGL,
and using the SDL library. Best results are in full-screen mode, however
windowed mode of any size should also work. Have fun!

Installation:
Unzip to a directory of your choice, and run asterad.exe.

Controls:
A           turn left
D           turn right
W           thrust
SPACE       fire
S           start, respawn
R           reset game
Q, Esc      quit
Alt-Enter   toggle windowed / full-screen

Some other less important keys include:
F2          toggle sound
F3          show bounding boxes
-           slow time (for the adolescent rush of "bullet time")
+           speed time
O           hit all asteroids (cheat)

When entering a high score:
A, D        scroll letters
S           enter letter

Tips:
* UFOs will break into pieces when shot, the largest pieces of which can destroy
  your ship. For this reason, be wary of attacking UFOs head-on.
* You score more points for daring / skilful play i.e. more points if you are
  flying fast, or shoot asteroids at short or long range.
* High scores are most easily attained by simply surviving more levels. As you
  progress, most of your points will come from end of level bonuses.

This game has been released under the GPL. See the file COPYING for details.

A port to other platforms (such as Linux, Mac etc) should be reasonably easy;
the author has neglected to make these simply because he only has a Windows
installation. Note that the following parts of code may currently be
Windows-specific:
 * dirent.h - other platforms should use their installed version of dirent.h
 * game.h and hiscore.h - a couple of directory defines use backslashes (\)
 * game.c - WinMain() is used instead of main()

Source can be found at http://github.com/gavinschultz/asterad.
