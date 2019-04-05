make: cube.c wizard.c
	gcc -g cube.c wizard.c -lreadline -lhistory -lncurses -lpthread -o cube
