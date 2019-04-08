Welcome to the Cube!

In order to run this file, just follow these steps:

	1. Run 'make'
	2. Run './cube' with the following arguments
		
		-size = The size of the cube
		-teamA = The amount of wizards on teamA
		-teamB = The amount of wizards on teamB
		-seed = The seed to base all random number generations off of
		
		An example command would look like this: ./cube -size 3 -teamA 2 -teamB 3 -seed 14
		In order to get start the CLI with a cube size of 3x3, 2 wizards on team A and 3 wizards on team B. The random number seed would be 14.

	3. Now you should be looking at a CLI that looks as follows:
		
		cube>
	
	And the commands that can be used to control the game are found below:
		
		- start = Initializes all threads, and prepares the game to be started (can only be done before starting the game, or after calling "stop")
		- stop = Kills all wizard threads, 
		- s = Allows one wizard to take a turn, and returns to the CLI
		- c = Runs through the rest of the game, only returning to the CLI after a winner has been determined and final board state has been printed
		- show = Prints the current state of the cube
		- exit = Exits the CLI
	
	If any unknown commands are used, you will be prompted to enter a correct one.