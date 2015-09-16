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
		error("Error: No such host");
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		error("Error: Could not create socket");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);
	serv_addr.sin_port = htons(portno);

	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1){
		error("Could not establish connection");
	}
    open_file_and_write(sockfd, filename);

	close(sockfd);
	return EXIT_SUCCESS;
}

void open_file_and_write (int sock, char * filename)
{

    int fhandle_open;
    int bytes_read;
    int bytes_written;
    char buffer[BUFSIZE];
    O_APPEND;
    FILE *fobj = fopen(filename, "w");
    fhandle_open = open(filename, O_RDWR, S_IRUSR | S_IWUSR);
    if (fhandle_open == -1){
    	error("Error opening file");

    }

    while (1){

    	bytes_read = recv(sock, (void *)buffer, sizeof(buffer),0);
    	printf("client: bytes read %d\n", bytes_read);
    	if (bytes_read == 0){
    		break;
    	}
    	else if (bytes_read < 0){
    		error("ERROR reading socket");
    	}
    	void *p = buffer;
    	while (bytes_read > 0){
    		bytes_written = write(fhandle_open, p, bytes_read);
    		printf("client: bytes written %d\n", bytes_written);
    		if (bytes_written <= 0){
    			error("ERROR writing to socket");
    		}
    		bytes_read -= bytes_written;
    		p+=bytes_written;
    	}
    }
	close(fhandle_open);
	fclose(fobj);
}

void error(const char *msg){
	perror(msg);
	exit(1);
}
