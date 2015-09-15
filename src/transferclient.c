#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#include <sys/stat.h>

#define BUFSIZE 4096

#define USAGE                                                                 \
"usage:\n"                                                                    \
"  transferclient [options]\n"                                                \
"options:\n"                                                                  \
"  -s                  Server (Default: localhost)\n"                         \
"  -p                  Port (Default: 8888)\n"                                \
"  -o                  Output file (Default foo.txt)\n"                       \
"  -h                  Show this help message\n"

void open_file_and_write (int sock, char * filename);
void error(const char *msg);
/* Main ========================================================= */
int main(int argc, char **argv) {
	int option_char = 0;
	int sockfd = 0; //socket file descriptor
	char *hostname = "localhost";
	unsigned short portno = 8888;
	char *filename = "foo.txt";
	struct hostent *server;
	struct sockaddr_in serv_addr;
	char buffer[256];

	// Parse and set command line arguments
	while ((option_char = getopt(argc, argv, "s:p:o:h")) != -1) {
		switch (option_char) {
			case 's': // server
				hostname = optarg;
				break;
			case 'p': // listen-port
				portno = atoi(optarg);
				break;
			case 'o': // filename
				filename = optarg;
				break;
			case 'h': // help
				fprintf(stdout, "%s", USAGE);
				exit(0);
				break;
			default:
				fprintf(stderr, "%s", USAGE);
				exit(1);
		}
	}

	/* Socket Code Here */
	server = gethostbyname(hostname);
	if(server == NULL){
		printf("Error: No such host\n");
		return EXIT_FAILURE;
	}
	//printf("gotserver\n");


	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error: Could not create socket\n");
		return EXIT_FAILURE;
	}
	//printf("created socket\n");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);
	serv_addr.sin_port = htons(portno);

	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1){
		printf("Could not establish connection\n");
		return EXIT_FAILURE;
	}
	bzero(buffer,256);
	//if(write(sockfd,buffer,strlen(buffer)) == -1){
	//	error("Write Failure");
	//}

    //if (read(sockfd,buffer,255) == -1)
    //     error("ERROR reading from socket");

    //printf("%s\n",buffer);
    open_file_and_write(sockfd, filename);
	close(sockfd);

	return EXIT_SUCCESS;
}

void open_file_and_write (int sock, char * filename)
{

    int fhandle_open;
    int number_bytes_read = 1;
    int number_bytes_write = 1;
    int bytes_read;
    int bytes_written;
    char buffer[100];
    O_APPEND;
    FILE *fobj = fopen(filename, "w");
    fhandle_open = open(filename, O_RDWR, S_IWRITE | S_IREAD);
    if (fhandle_open == -1){
    	error("Error opening file");

    }

    while (1){

    	bytes_read = read(sock, (void *)buffer, sizeof(buffer));
    	if (bytes_read == 0){
    		break;
    	}
    	else if (bytes_read < 0){
    		error("ERROR reading socket");
    	}
    	void *p = buffer;
    	while (bytes_read > 0){
    		bytes_written = write(fhandle_open, p, bytes_read);
    		if (bytes_written <= 0){
    			error("ERROR writing to socket");
    		}
    		bytes_read -= bytes_written;
    		p+=bytes_written;
    	}
		//number_bytes_write = write(fhandle_open, (void *)buffer, sizeof(buffer));
		//if (number_bytes_write == -1){
		//	error("Error writing");
		//}
    }
	close(fhandle_open);
	close(fobj);
}

void error(const char *msg){
	perror(msg);
	exit(1);
}
