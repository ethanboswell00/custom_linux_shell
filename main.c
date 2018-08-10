/*

	Custom-Made Linux Shell

	Copyright 2018 Ethan Boswell Coding LLC.

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>

#define MAX_ARGS 100

int checkForSpecialCmd(char **);

int main(int argc, char *argv[]) {
	char user_command[1000];
	char path[PATH_MAX + 1];
	char *pathname, *name = getenv("USER");
	char *token, **tokens;
	char *cd_pattern = " ";

	pid_t wpid, pid;
	int i, status, specialCmd, waitVar = 1, numSpaces = 0;

	tokens = (char **) malloc(sizeof(char *) * MAX_ARGS);

	for (i = 0; i < 50; i++) {
		tokens[i] = (char *) malloc(sizeof(char) * 1000);
	}

	token = (char *) malloc(sizeof(char) * 1000);

	while (1) {
		numSpaces = 0;
		user_command[0] = '\0';
		pathname = getcwd(path, PATH_MAX + 1);

		// Forked process prints in front of prompt, waits for
		// 'enter' from user to show prompt after process
		if (waitVar % 2 == 0) getchar();
		printf("Current Dir: %s\n", pathname);
		printf("%s: $ ", name);
		fgets(user_command, 1000, stdin);

		for (i = 0; i < strlen(user_command); i++) {
			if (user_command[i] == ' ') numSpaces++;
		}

		token = strtok(user_command, cd_pattern);
		tokens[0] = token;

		// Set args in 2d array
		if (numSpaces >= 1) {
			for (i = 0; i < numSpaces; i++) {
				token = strtok(NULL, cd_pattern);
				tokens[i + 1] = token;
			}
		}

		if (tokens[0][0] == '\n') continue;

		// Remove the '\n' character from the last arg
		tokens[numSpaces][strlen(tokens[numSpaces]) - 1] = '\0';
		tokens[numSpaces + 1] = NULL;

		specialCmd = checkForSpecialCmd(tokens);

		if (specialCmd != 0) {
			if (waitVar % 2 == 0) waitVar++;

			if (specialCmd == 1) break;
			else if (specialCmd == 2) continue;
		}

		pid = fork();
		waitVar++;

		if (pid == 0) {
			if (execvp(tokens[0], tokens) < 0) {
				perror("Error executing process.\n");
			}

			exit(1);
		} else if (pid < 0) {
			perror("Error forking child process.\n");
		} else {
			while (!WIFEXITED(status) && !WIFSIGNALED(status)) {
				wpid = waitpid(pid, &status, WUNTRACED);
			}
		}
	}

	return 0;
}

int checkForSpecialCmd(char **token) {
	if (!(strcmp(token[0], "exit"))) return 1;
	else if (!(strcmp(token[0], "cd"))) {
		chdir(token[1]);
		return 2;
	} else return 0;
}
