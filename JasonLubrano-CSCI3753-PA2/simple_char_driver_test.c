#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>

#define DEVICENAME "/dev/simple_character_device"
#define BUFF_SIZE 1024

int main(){
	char command;
	int length, whence, new_offset;
	char buffer[BUFF_SIZE];
	int file = open(DEVICENAME, O_RDWR);
	bool running = true;
	while(running){
		printf("\n\nJason's Program\n");
		printf("COMMANDS:\n");
		printf("	'r' to read from device\n");
		printf("	'w' to write to device\n");
		printf("	's' to seek from device\n");
		printf("	'e' to exit from device\n");
		printf("	anything else brings up main menu\n");
		printf("/SCD/command$> ");
		scanf("%c", &command);

		switch(command){
			case 'r':
				printf("/SCD/read$> How many bytes to read: "); /* ask user how many bytes */
				scanf("%d", &length); /* user inputs length of how many bytes */
				read(file, buffer, length); /* reads from the file, puts it to the buffer for x-length */
				printf("/SCD/read$> %s\n", buffer); /* prints the buffer */
				while(getchar() != '\n'); /* while its not a new line, continue */
				break;
			case 'w':
				printf("/SCD/write$> "); /* User writes to the file*/
				//fgets(buffer, sizeof(buffer), sdtin); /* get the user strings */
				/* fgets didnt work so I will ahve to do write
					only problem with that is that it is stopping
					at the first whitespace. anything else gave me a
					segmentation fault.*/
				scanf("%s", buffer);
				write(file, buffer, BUFF_SIZE); /* writes the buffer to file */
				while (getchar() != '\n'); /* while its not a new line, continue */
				break;
			case 's':
				printf("SEEK COMMANDS:\n");
				printf("	'0' seek set\n");
				printf("	'1' seek cur\n");
				printf("	'2' seek end\n");
				printf("	anything else brings up main menu\n");
				printf("/SCD/seek$> Enter whence: ");
				scanf("%d", &whence);
				printf("\n/SCD/write$> Enter an offset value: ");
				scanf("%d", &new_offset);
				llseek(file, new_offset, whence);
				break;
			case 'e':
				printf("/SCD/exit$> Exiting\n");
				running = false;
				break;
			default:
				printf("\n/SCD/error$> error: not a valid command\n");
				break;
		}
	}
	close(file);
	return 0;
}