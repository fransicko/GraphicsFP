Khanh Duong - kduong@mymail.mines.edu / Tazuzu
A7 - Avoid the Park Mascots

Added enemy marble mascots to the scene. Mascot will follow Suzanne and turn at an incremented
angle to simulate arc. Mascot moves slower than Suzanne, allowing the player to outrun them.
Colliding with them will result in the player's death. Marble enemies can be killed off by
baiting them off the ledge or having them collide with each other.

The camera can be adjusted with the left click and zoom with CTRL + left click or scroll wheel.
Suzanne can be moved using WASD. She is  no longer bounded to the platform, running off will
result in a death.

There are colorful walls on the platform. The solid walls are particularly sticky and will stop
all movements coming towards them for both the player and the enemies. Strategizing with the
walls will make the map much easier. Objective of the game is to reach the colorful wired
cube, which will win the game.

To run, make and run the resulting .exe
	>a7.exe
The Makefile attached can be used to compile the code. It has been adjusted for home use,
so be sure to update to your correct paths or set lab value to 0.
The game also comes with 2 control files:
	systems.csv places the enemy spawners and particle systems.
	walls.csv places the colorful wall cube as well as the winning wired cube.
Feel free to make your own level.

My original plan was to implement the "sliding" effect that was shown on the demo when
objects run towards the walls. It was too painful to think of an implementation which
resulted in the sticky walls.

This assignment took 10 hours.
I wish I was better at OpenGL, there are things I wished I knew how to do but it just takes
too much time and commitment at this part of the semester.
The Marble class made this alot easier. 8/10.
Fun factor: 7/10.