#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

// all socket related packages
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>

#include "trie.h"

#define PORT "6969" // the port users will connect to
#define BACKLOG 10    // how many pending connections will queue
#define MAXLINE 4096 // max read from socket HTTP request

// used for passing info into each pthread
typedef struct AddrInfo {
	Trie *trie_head;
	int socket_ptr; // just need the file point
	int *thread_status;
} meta_info_t;

meta_info_t *meta_info_create(Trie *trie, int socket_ptr, int *thread_status_ptr) {
	meta_info_t *new_meta_info = malloc(sizeof(meta_info_t));

	// insert all data
	new_meta_info->trie_head = trie;
	new_meta_info->socket_ptr = socket_ptr;
	new_meta_info->thread_status = thread_status_ptr;

	return new_meta_info;
}

int compare(float num1, float num2) {
	return num1 < num2;
}

void sigchld_handler(int s) {
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa) {

	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

void *acceptor_function(void *sock_ptr);

// reads all the text in a file into an allocated char *
char *readpage(char *filename, int *length) {
	FILE *file = fopen(filename, "r");

	if (!file)
		// error case -- assumes error.html exists
		file = fopen("./views/error.html", "r");

	size_t *bufferweight = malloc(sizeof(size_t));
	*bufferweight = 0;
	char *getlinestr = "";
	char *buildstring = "";

	int reallinelen;

	while ((reallinelen = getline(&getlinestr, bufferweight, file)) > 0) {
		
		int new_buildsize = sizeof(char) * (strlen(buildstring) + 1) + sizeof(char) * strlen(getlinestr);
		if (strlen(buildstring) == 0) {
			buildstring = malloc(new_buildsize);
			strcpy(buildstring, getlinestr);
			buildstring[new_buildsize - 1] = '\0';
		} else {
			buildstring = realloc(buildstring, new_buildsize);
			strcpy(buildstring + (sizeof(char) * strlen(buildstring)), getlinestr);
		}

		*length += reallinelen;
		*bufferweight = 0;
	}

	// copy buildstring over with added size
	// second portion for the socket sending
	int page_size = snprintf(NULL, 0, "%d", *length);
	int html_chars = *length;

	// calculate length of send
	*length = sizeof(char) * html_chars + sizeof(char) * (59 + page_size);
	char *returnstring = malloc(sizeof(char) * *length);

	// copy in the size
	sprintf(returnstring, "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: %d\n\n\n", html_chars);
	// copy in buildstring (moving the starte over by the amount currently in returnstring)
	strcpy(returnstring + sizeof(char) * (58 + page_size), buildstring);

	free(bufferweight);
	free(getlinestr);
	free(buildstring);

	return returnstring;
}

int load_trie(Trie *trie, char *filename) {
	FILE *file = fopen(filename, "r");

	if (!file) {
		return 0;
	}

	size_t size = sizeof(char) * 80;
	char *reader = malloc(size);

	int counter = 0;

	while (getline(&reader, &size, file) != -1) {
		char *word = malloc(sizeof(char) * 30);
		double word_freq = 0;

		sscanf(reader, "%s %lf", word, &word_freq);

		insert(trie, word, word_freq, 0);
		counter += word_freq;

		free(word);
	}

	free(reader);
	fclose(file);

	return 0;
}

// this counts the weight at this specific layer
// and counts the weight for further down layers
double max_weight(Trie **children) {
	double max_sum = 0;

	for (int check_sums = 0; check_sums < 26; check_sums++) {
		if (children[check_sums])
			max_sum += children[check_sums]->weight + children[check_sums]->next_weight;
	}

	return max_sum;
}

char *getword(Trie *trie, char *res, int curr_index, int *max_length) {
	double max = max_weight(trie->children);

	double random_number0_1 = (double) rand() / (double) RAND_MAX;
	double trie_pos = max * random_number0_1;

	// search for the random position based on trie_pos and the weights
	// at children and next children
	double current_weight = 0;
	int find_pos;
	int current_trie_next = 0; // decides if the search continues
							   // or if the node was found
	for (find_pos = 0; find_pos < 26; find_pos++) {
		if (!trie->children[find_pos])
			continue;
		// first check weight at this specific point
		current_weight += trie->children[find_pos]->weight;

		// see if this weight jumps over trie_pos
		if (current_weight > trie_pos) {
			current_trie_next = 1;
			break;
		}

		// check next weights
		current_weight += trie->children[find_pos]->next_weight;

		if (current_weight > trie_pos)
			break;
	}

	if (find_pos >= 26) // return before trying to add
		return res;

	res[curr_index] = (char) (find_pos + 97);

	curr_index++;
	*max_length += 1;
	res = realloc(res, sizeof(char) * *max_length);

	res[curr_index] = '\0';

	if (current_trie_next) // return
		return res;

	return getword(trie->children[find_pos], res, curr_index, max_length);
}

int main() {
	Trie *trie = childTrie();
	load_trie(trie, "norvigclean.txt");

	srand(time(NULL));

	// connection stuff
	int sock_fd; // listen on sock_fd
	int *thread_status = malloc(sizeof(int)); // checks on if ALL threads should be closed
	*thread_status = 1; // Good to go!
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	struct sigaction sa;
	int yes=1;
	int status;

	memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;	  // dont' care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;	  // fill in my IP for me

	if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}


	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	// find a working socket
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sock_fd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sock_fd);
			perror("server: bind");
			continue;
		}

		break;
	}


	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sock_fd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	printf("server go vroom\n");

	meta_info_t *acceptor_data = meta_info_create(trie, sock_fd, thread_status);

	// split acceptor
	pthread_t accept_thread;
	int check = pthread_create(&accept_thread, NULL, &acceptor_function, acceptor_data);

	// wait until a user tries to close the server
	while(getchar() != '0');
	// to close all client threads, use the thread_status pointer
	*thread_status = 0; // off!
	pthread_cancel(accept_thread);
	pthread_join(accept_thread, NULL);

	destruct(trie);
	free(thread_status);
	free(acceptor_data);

	return 0;
}

char *parse_http(char *full_req) {

	int find_req_url = 0;

	while ((int) full_req[find_req_url] != 32)
		find_req_url++;

	find_req_url++;

	int req_url_size = 8;
	int req_url_index = 0;
	char *req_url = malloc(sizeof(char) * req_url_size);

	while ((int) full_req[find_req_url + req_url_index] != 32) {
		req_url[req_url_index] = full_req[find_req_url + req_url_index];

		req_url_index++;

		if (req_url_index == req_url_size) {
			req_url_size *= 2;
			req_url = realloc(req_url, sizeof(char) * req_url_size);
		}
	}

	req_url[req_url_index] = '\0';

	return req_url;
}

int send_page(int new_fd, char *request, Trie *trie_head) {
	int *res_length = malloc(sizeof(int));
	int res_sent;
	*res_length = 0;

	char *res;
	// homepage
	if (strcmp(request, "/") == 0)
		res = readpage("./views/homepage.html", res_length);
	else if (strcmp(request, "/typingtest") == 0)
		res = readpage("./views/type.html", res_length);
	else if (strcmp(request, "/newword") == 0) {
		*res_length = 1;
		res = malloc(sizeof(char));
		char *word = malloc(sizeof(char));
		word[0] = '\0';

		word = getword(trie_head, word, 0, res_length);
		int page_size = snprintf(NULL, 0, "%d", *res_length);

		*res_length = sizeof(char) * *res_length + sizeof(char) * (page_size + 60);
		res = realloc(res, *res_length);

		// copy in the size
		sprintf(res, "HTTP/1.1 200 OK\nContent-Type:text/plain\nContent-Length: %d\n\n\n", *res_length);
		// copy in buildstring (moving the starte over by the amount currently in returnstring)
		strcpy(res + sizeof(char) * (60 + page_size), word);

		printf("send response %s\n", res);

		free(word);
	} else
		res = readpage("./views/error.html", res_length);

	// use for making sure the entire page is sent
	while ((res_sent = send(new_fd, res, *res_length, 0)) < *res_length);

	printf("sent\n");

	free(res_length);
	free(res);

	return 0;
}

void *connection(void *addr_input) {
	meta_info_t *client_data = (meta_info_t *) addr_input;

	int new_fd = client_data->socket_ptr;

	int recv_res = 1;
	char *buffer = malloc(sizeof(char) * MAXLINE);
	int buffer_len = MAXLINE;

	// make a continuous loop for new_fd while they are still alive
	// as well as checking that the server is running
	while (recv_res = recv(new_fd, buffer, buffer_len, 0) && *client_data->thread_status) {
		if (recv_res == -1) {
			perror("receive: ");
			continue;
		}

		if (buffer_len == 0)
			continue;

		// otherwise we have data!
		char *request = parse_http(buffer);
		send_page(new_fd, request, client_data->trie_head);
	}

	// if the close occurs due to thread_status, send an error page
	if (!*client_data->thread_status)
		send_page(new_fd, "/error", client_data->trie_head);

	close(new_fd);
	free(buffer);
	free(client_data);

	pthread_t *retval;
	// close thread
	pthread_exit(retval);
}

void *acceptor_function(void *sock_ptr) {
	meta_info_t *acceptor_data = (meta_info_t *) sock_ptr;

	int sock_fd = acceptor_data->socket_ptr;
	int new_fd;
	char s[INET6_ADDRSTRLEN];
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;

	printf("listening");

	while (1) {
		sin_size = sizeof(their_addr);
		new_fd = accept(sock_fd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *) &their_addr),
		s, sizeof s);
		printf("server: got connection from %s\n", s);

		// create new meta_info
		meta_info_t *client_socket_data = meta_info_create(acceptor_data->trie_head, new_fd, acceptor_data->thread_status);

		// at this point we can send the user into their own thread
		pthread_t socket;
		pthread_create(&socket, NULL, &connection, client_socket_data);
	}

	return 0;
}