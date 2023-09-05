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
#include "./http-1.0/http.h"



int main(int argc, char* argv[]){

        const int PORT = atoi(argv[1]);



/*

        const char* message = argc > 2 ? argv[2] : "Hello there!";
        char buffer[BUFFER_SIZE] = {0};

        send(new_socket, message, strlen(message), 0);

        recv(new_socket, buffer, BUFFER_SIZE, 0);
        printf("Client said '%s'\n", buffer, 0);


        send(new_socket, buffer, strlen(buffer), 0);

        recv(new_socket, buffer, BUFFER_SIZE, 0);
        printf("Client said '%s'\n", buffer);

        send(new_socket, buffer, strlen(buffer), 0);

        close(new_socket);
        close(server_sockfd);
*/
	

		int server_sockfd = start_http_server(PORT);

		HttpRequest* req = accept_http_request();
		print_http_request(req);		
        printf("Request : %s\n", req->body);
		
		HttpResponse* res = new_http_response();


		 set_http_response_header(res, "Status", "200");
         set_http_response_header(res, "Status-Message", "OK");
         set_http_response_header(res, "Version", "http/1.0");
         set_http_response_header(res, "Content-Type", "text/json");
         set_http_response_header(res, "Connection", "close");
         set_http_response_header(res, "Date-Time", "02/09/2023");


		sprintf(res->body, "Request made to endpoint : %s\n", req->path);
		
		print_http_response(res);
		send_http_response(req, res);



		close(server_sockfd);



        return 0;
}
