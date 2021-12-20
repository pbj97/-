#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#define DEFAULT_PROTOCOL 0
#define MAXLINE 100
#include "locker.h"

int readLine(int fd, char* str);

int main(int argc, char *argv[]){

	int listenfd, connfd, clientlen, n1, n2,n3,i;
	struct sockaddr_un serverUNIXaddr, clientUNIXaddr;

	char inmsg1[MAXLINE], inmsg2[MAXLINE],inmsg3[MAXLINE],inmsg4[MAXLINE];;
	char outmsg1[MAXLINE], outmsg2[MAXLINE],outmsg3[MAXLINE],outmsg4[MAXLINE];
	char current_state[MAXLINE];
	char temp[MAXLINE];

	struct locker *record;
	int locker_num;
	int idx;
	char password_digit[MAXLINE];

	int digit;

	signal(SIGCHLD, SIG_IGN);
	clientlen = sizeof(clientUNIXaddr);

	listenfd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
	serverUNIXaddr.sun_family = AF_UNIX;

	strcpy(serverUNIXaddr.sun_path, "convert");
	unlink("convert");
	bind(listenfd, &serverUNIXaddr, sizeof(serverUNIXaddr));

	listen(listenfd, 5);

	printf("set the number of locker: ");
	scanf("%d", &locker_num);
	record = (struct locker *) malloc(locker_num * sizeof(struct locker));

	for(i = 0; i <locker_num; i++)
	{
		sprintf(record[i].is_available, "empty");
	}
	printf("set the password digit: ");
	scanf("%s",password_digit);


	while (1) {
		connfd = accept(listenfd, &clientUNIXaddr, &clientlen);
		if (fork() == 0) {
			while (1) {

				write(connfd, password_digit,strlen(password_digit)+1);
				digit = atoi(password_digit);


				current_state[0]= '\0';

				for (i = 0; i < locker_num; i++) {
					sprintf(temp, "%d: %s ", i + 1, record[i].is_available);
					strcat(current_state, temp);
				}

				sleep(1);

				write(connfd, current_state, strlen(current_state) + 1);



				n1 = read(connfd, inmsg1, MAXLINE);
				idx = atoi(inmsg1);

				if (idx <= 0 || idx > locker_num) {
					sprintf(outmsg2, "Error: number should be in 1 to %d\n",
							locker_num);
					write(connfd, outmsg2, strlen(outmsg2) + 1);
					sleep(1);
					continue;
				} else {
					if (record[idx - 1].is_available[0] == 'e') {

						sprintf(outmsg2, "set password (%d digits): ",
								digit);
						write(connfd, outmsg2, strlen(outmsg2) + 1);
						n2 = read(connfd, inmsg2, MAXLINE);

						for (i = 0; i < digit; i++) {
							record[idx - 1].password[i] = inmsg2[i];
						}
						sprintf(record[idx - 1].is_available, "occupied");
						continue;
					}
					else
					{
						while (1) {

							sprintf(outmsg2, "insert password (%d digits): ",digit);

							write(connfd, outmsg2, strlen(outmsg2) + 1);
							n2 = read(connfd, inmsg2, MAXLINE);
							if (is_correct(inmsg2, record[idx - 1].password,digit)) {
								sprintf(outmsg3,
										"Select menu(1: unlock 2: change password others: exit): ");
								write(connfd, outmsg3, strlen(outmsg3) + 1);

								n3 = read(connfd, inmsg3, MAXLINE);
								if (inmsg3[0] == '1') {
									sprintf(record[idx - 1].is_available,
											"empty");
									break;
								} else if (inmsg3[0] == '2') {
									sprintf(outmsg4,"Input new password: ");
									write(connfd, outmsg4, strlen(outmsg4) + 1);
									n2 = read(connfd, inmsg4, MAXLINE);

									for (i = 0; i < digit; i++) {
										record[idx - 1].password[i] = inmsg4[i];
									}
									break;

								} else {
									break;
								}

							} else {
								sprintf(outmsg3, "Wrong password try again\n");
								write(connfd, outmsg3, strlen(outmsg3) + 1);
								continue;
							}

						}

					}

				}
			}

		} else
			close(connfd);
	}
	free(record);
	exit(0);


}

int is_correct(char arr[],char arr2[],int num){
	int i;

	for(i=0;i<num;i++){
		if(arr[i] != arr2[i])
			return 0;
	}
	return 1;
}

int readLine(int fd, char* str)
{
	int n;
	do {
		n = read(fd, str, 1);
	} while(n > 0 && *str++ != NULL);
	return(n > 0);
}
