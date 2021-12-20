#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#define DEFAULT_PROTOCOL 0
#define MAXLINE 100

int readLine(int fd, char *str);

int main() {
	struct flock lock;

	int locker_num, pass, correct_pass;
	char password_digit[MAXLINE];
	char outmsg1[MAXLINE], outmsg2[MAXLINE], outmsg3[MAXLINE], outmsg4[MAXLINE];
	char inmsg1[MAXLINE], inmsg2[MAXLINE], inmsg3[MAXLINE], inmsg4[MAXLINE],
			ans[2];

	int digit;

	int clientfd, result;

	int n1, n2, n3, i, count = 0;

	struct sockaddr_un serverUNIXaddr;

	clientfd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);

	serverUNIXaddr.sun_family = AF_UNIX;

	int temp;

	strcpy(serverUNIXaddr.sun_path, "convert");

	do { /* 연결 요청 */
		result = connect(clientfd, &serverUNIXaddr, sizeof(serverUNIXaddr));
		if (result == -1)
			sleep(1);
	} while (result == -1);

	while (1) {
		n1 = read(clientfd, password_digit, MAXLINE);
		digit = atoi(password_digit);

		// cabinet info
		n1 = read(clientfd, inmsg1, MAXLINE);
		printf("%s\n", inmsg1);

		// select locker number
		printf("\n select locker: ");
		scanf("%d", &locker_num);
		sprintf(outmsg1, "%d", locker_num);
		write(clientfd, outmsg1, strlen(outmsg1) + 1);

		// call lock status
		n2 = read(clientfd, inmsg2, MAXLINE);
		if (inmsg2[0] == 's') { // first visit lock

			while (1) {
				count = 0;
				printf("%s", inmsg2);
				scanf("%d", &pass);
				temp = pass;

				while (temp != 0) {
					temp = temp / 10;
					++count;
				}
				if (count == digit) {
					break;
				} else {
					printf("wrong password digit(%d digits).\n", digit);
					continue;

				}
			}

			while (1) { // pass double check
				printf("\n write your password again: ");
				scanf("%d", &correct_pass);
				if (pass == correct_pass)
					break;
				else
					continue;
			}
			sprintf(outmsg2, "%d", correct_pass);
			write(clientfd, outmsg2, strlen(outmsg2) + 1);
			printf("Set\n");
		} else if (inmsg2[0] == 'E') {
			printf("%s", inmsg2);
		} else { //second visit
			while (1) {

				printf("%s", inmsg2);
				scanf("%d", &pass);
				sprintf(outmsg2, "%d", pass);
				write(clientfd, outmsg2, strlen(outmsg2) + 1);

				n3 = read(clientfd, inmsg3, MAXLINE);
				if (inmsg3[0] == 'S') //correct
					{
					printf("%s", inmsg3);
					scanf("%s", &ans);
					sprintf(outmsg3, "%s", ans);
					write(clientfd, outmsg3, strlen(outmsg3));

					if (ans[0] == '1') {
						printf("set\n");
						break;
					}
					else if (ans[0] == '2')
					{
						n3 = read(clientfd, inmsg4, MAXLINE);
						printf("%s", inmsg4);
						while (1) {
							count = 0;
							printf("%s", inmsg2);
							scanf("%d", &pass);
							temp = pass;

							while (temp != 0) {
								temp = temp / 10;
								++count;
							}
							if (count == digit) {
								break;
							} else {
								printf("wrong password digit(%d digits).\n",
										digit);
								continue;

							}
						}

						while (1) { // pass double check
							printf("\n write your password again: ");
							scanf("%d", &correct_pass);
							if (pass == correct_pass)
								break;
							else
								continue;
						}
						sprintf(outmsg4, "%d", correct_pass);
						write(clientfd, outmsg4, strlen(outmsg4) + 1);
						printf("Set\n");
						break;
					}
					else{
						break;
					}
				} else // wrong
				{
					printf("%s", inmsg3);
					continue;
				}

			}
		}

	}
	close(clientfd);

	return 0;

}

int readLine(int fd, char *str) {
	int n;
	do {
		n = read(fd, str, 1);
	} while (n > 0 && *str++ != NULL);
	return (n > 0);
}
