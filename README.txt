Final Project - Space Derby
The Dank Squad
Khanh Duong - kduong@mymail.mines.edu / Tazuzu
Truyen Van - tvan@mymail.mines.edu / Kat
Michael Villafuerte - mvillafu@mymail.mines.edu / Fransicko

Program is game where player tries to dodge the meteors to reach the star.
Players must also avoid falling off the edge of the track.

Controls
	A / D : to move Left and Right to dodge
	1 : Arcball cam, if players would like to control camera themself
		Ctrl + Left click to zoom
		Left click to turn camera
	2 : Overhead view, straight down view. Some might find it easier to orientate
	
User Interaction
	A / D was chosen to as the control format because it is intuitive for these types
	of game. Using the full WASD felt awkward, and it was a team decision to simplify
	the controls. Camera's can still be operated like before.
	
Animation
	Suzanne is the model for the player. Suzanne wiggles as she move. On top of that,
	Suzanne is fitted with two space wheels that was built using hierarchical modeling
	which rotates as she moves.
	
Texturing
	We used a lot of textures for this project
		1) Suzanne has a diamond texture
		2) Suzanne's wheels are rubber texture
		3) There is a space skybox
		4) There is a fire particle system
		5) The orbiting sun has a sun texture
		6) Meteors surfaces have a rocky texture
		7) The road has an asphalt texture
		
Lighting
	The first light we have is from the star at the end of the platform. It shines light towards
	the players direction.
	The second light is from the orbiting star around Suzanne. It light is dynamic as it rotates.
	Both lights affect Suzanne and the meteors.
	
Shader Programming
	1) Pair of Shaders to Light / Texture Player Model + Wheels
	2) Pair of Shaders to Light / Texture Obstacles (Meteors)
	3) Pair of Shaders + Geo Shader for bill boarding Particle System
	4) Pair of Shaders for Skybox
	5) Pair of Shaders for post processing.
		a)	Unique Vertex Shader
			The frame buffer render progressively gets smaller to increase difficulty, and
			to somewhat simulate distance. The vertex position is being modify based on the distance
			player is away from the sun.
		b)	Unique Fragment Shader
			This shader lights the entire screen as the player approaches the sun, to simulate 
			the brightness. Getting close to the sun with be glaringly bright, until the 
			entire screen is flooded, indicating the player has reached the end.
		
To Run:
	make the file with the included makefile, adjust directory if needed
		run the fp.exe
		
Input Files:
	systems.csv
	Systems input file creates the particle systems and the spawners for the meteors
	Players may choose to add / remove spawners to change the game
	Note that some setup of the spawners will make the game unbeatable, so for now,
	just use the suggested Spawners.
	The particle systems are purely for cosmetic and can be adjusted if you wish.
	
	For Particle Systems
	F, <x>, <y>, <z>, <coneAngle>, <minVelocity>, <maxVelocity>, <minLife>, <maxLife>, <spawnRate>
	
	For Meteor Spawners
	S, <x>, <y>, <z>, <dirX>, <dirY>, <dirZ>, <size>, <spawnRate>
	
Responsibilities
	Khanh: Core functionality/control of the game, particle systems, lighting
	Michael: Texture, animation
	Truyen: Post processing shader, skybox, dying conditions
		
This assignment took us about 20 hours
Lab helped alot for this project: 10/10
Assignment was pretty fun: 8/10

Bugs: Some issue with lighting, the glare at center of meteors. Not sure if intended
	