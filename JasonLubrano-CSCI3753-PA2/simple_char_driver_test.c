#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>

#define DEVICE "/dev/simple_character_device"
#define BUFF_SIZE 1024

int main(){
	char command;
	char buffer[BUFF_SIZE];
	int file = open(DEVICE, O_RDWR);
	bool running = true;
	while(running){
		printf("COMMANDS:\n");
		printf("	'r' to read from device\n");
		printf("	'w' to write from device\n");
		printf("	's' to seek from device\n");
		printf("	'e' to exit from device\n");
		printf("	anything else to keep reading/writing\n");
		printf("$> ");
		scanf("%c", &command);

		switch(command){
			case 'r': read(file, buffer, BUFF_SIZE);
				printf("\nr> %s\n", buffer);
				while(getchar() != '\n');
				break;
			case 'w': printf("\nw> ");
				gets(buffer);
				write(file, buffer, BUFF_SIZE);
				while(getchar() != '\n');
				break;
			case 's': printf("\ns>\n");
				break;
			case 'e': printf("\ne> Exiting\n");
				running = false;
				break;
			default: printf("\n$> error: not a valid command\n");
				break;
		}
	}
	close(file);
	return 0;
}