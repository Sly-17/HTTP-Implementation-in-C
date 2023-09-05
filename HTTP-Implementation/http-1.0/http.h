#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>

#define BUFFER_SIZE 200
#define SMALL_BUFFER 20
#define BUFFER_10 10
#define BUFFER_20 20
#define BUFFER_100 100
#define BUFFER_200 200

#define one_KB 1024
#define two_MB 2*1024*1024  
// Ensure response/request is streamed in chunks in case of large request/response header/body sizes; this implementations streams in request and response bodies in 1KB chunks
// Check https://stackoverflow.com/questions/16990746/isnt-recv-in-c-socket-programming-blocking




typedef struct{
		char method[BUFFER_10];
		char path[BUFFER_200];
		char version[BUFFER_10];
		char accept[BUFFER_200];
		char connection[BUFFER_20];
		char user_agent[BUFFER_100];
		int sockfd;
		char body[two_MB];
} HttpRequest;


typedef struct{
		int status;
		char status_message[BUFFER_200];
		char version[BUFFER_10];
		char connection[BUFFER_20];
		char content_type[BUFFER_200];
		char datetime[BUFFER_100];
		int sockfd;
		char body[two_MB];
} HttpResponse;


void error_and_die(const char* error){
		printf("ERROR : %s\n", error);
		exit(1);
}


int http_min(int a, int b){ return a < b ? a : b;}


HttpRequest* new_http_request(){
		HttpRequest* req = (HttpRequest*)(malloc(sizeof(HttpRequest)));
		return req;
}


HttpResponse* new_http_response(){
		HttpResponse* res = (HttpResponse*)(malloc(sizeof(HttpResponse)));
		return res;
}


void set_http_request_header(HttpRequest* req, const char* param, const char* value){
		if(strcmp(param, "Method") == 0){
				strcpy(req->method, value);
		}else if(strcmp(param, "Path") == 0){
				strcpy(req->path, value);
		}else if(strcmp(param, "Version") == 0){
				strcpy(req->version, value);
		}else if(strcmp(param, "Accept") == 0){
				strcpy(req->accept, value);
		}else if(strcmp(param, "Connection") == 0){
				strcpy(req->connection, value);
		}else if(strcmp(param, "User-Agent") == 0){
				strcpy(req->user_agent, value);
		}

}


void set_http_response_header(HttpResponse* res, const char* param, const char* value){
		if(strcmp(param, "Status") == 0){
				res->status = atoi(value);
		}else if(strcmp(param, "Status-Message") == 0){
				strcpy(res->status_message, value);
		}else if(strcmp(param, "Version") == 0){
				strcpy(res->version, value);
		}else if(strcmp(param, "Content-Type") == 0){
				strcpy(res->content_type, value);
		}else if(strcmp(param, "Connection") == 0){
				strcpy(res->connection, value);
		}else if(strcmp(param, "Date-Time") == 0){
				strcpy(res->datetime, value);
		}

}




void print_http_response(HttpResponse* res){
		printf("RESPONSE HEADERS\n");

		printf("Status         : %d\n", res->status);
		printf("Status Message : %s\n", res->status_message);
		printf("Version        : %s\n", res->version);
		printf("Connection     : %s\n", res->connection);
		printf("Content-Type   : %s\n", res->content_type);
		printf("Date-Time      : %s\n", res->datetime);
		
		
		printf("\n\n\n");
}	




void print_http_request(HttpRequest* req){
		printf("REQUEST HEADERS\n");
	
		printf("Method         : %s\n", req->method);
		printf("Path           : %s\n", req->path);
		printf("Version        : %s\n", req->version);
		printf("Accept         : %s\n", req->accept);
		printf("Connection     : %s\n", req->connection);
		printf("User-Agent     : %s\n", req->user_agent);
		
		printf("\n\n\n");
}


int server_sockfd;
struct sockaddr_in address;
int PORT = 9300;

HttpResponse* make_http_request(HttpRequest* req, int PORT){

		struct sockaddr_in server_address;
		char buffer[BUFFER_SIZE] = {0};

		int client_sockfd;
		if((client_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				error_and_die("Could not create socket!!");
		}

		req->sockfd = client_sockfd;


		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(PORT);
		server_address.sin_addr.s_addr = inet_addr("127.0.0.1");


		if(connect(client_sockfd, (struct sockaddr*)(&server_address), sizeof(server_address)) < 0){
				error_and_die("Connect Error");
		}

		send(client_sockfd, req, sizeof(HttpRequest), 0);

		HttpResponse* res = (HttpResponse*)(malloc(sizeof(HttpResponse)));
		recv(client_sockfd, res, sizeof(HttpResponse) - two_MB, 0);

		int chunks = 0;
		while(recv(client_sockfd, res->body + chunks * one_KB, one_KB, 0)) chunks++;

		close(client_sockfd);

		return res;

}

int start_http_server(const int port){
		PORT = port;

		if((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				error_and_die("Could not create socket");
		}

		address.sin_family = AF_INET;
		address.sin_port = htons(PORT);
		address.sin_addr.s_addr = htonl(INADDR_ANY);


		// Bind socket to PORT
		if(bind(server_sockfd, (struct sockaddr*)(&address), sizeof(address)) < 0){
				error_and_die("Bind error");
		}

		// Listen from PORT
		if(listen(server_sockfd, 5) < 0){
				error_and_die("Listen Error");
		}

		return server_sockfd;
}


HttpRequest* accept_http_request(){

		// Accept connection
		int new_socket;
		int addr_size = sizeof(address);

		if((new_socket = accept(server_sockfd, (struct sockaddr*)(&address), (socklen_t*)(&addr_size))) < 0){
				error_and_die("Accept Error");
		}

		HttpRequest* req = (HttpRequest*)(malloc(sizeof(HttpRequest)));
		recv(new_socket, req, sizeof(HttpRequest), 0);

		//int chunks = 0;
		//while(recv(new_socket, req->body + chunks * one_KB, one_KB, 0)) chunks++;

		req->sockfd = new_socket;
		return req;

}

// HTTP response `res` corresponding to HTTP request `req`
void send_http_response(HttpRequest* req, HttpResponse* res){
		send(req->sockfd, res, sizeof(HttpResponse), 0);	
		close(req->sockfd);
}

void close_http_server(int sockfd){
	close(sockfd);
}
