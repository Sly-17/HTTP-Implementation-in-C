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



/*
   void set_http_body(HttpRequest* req, void* body){
// Not creating a copy of body here as the body will remain atleast as long as the request is finished
req->body = body;
}
 */



int main(int argc, char* argv[]){

		const int PORT = atoi(argv[1]);

		/*
		   recv(client_sockfd, buffer, BUFFER_SIZE, 0);
		   printf("Server says '%s'\n", buffer);

		   strcpy(buffer, "Message 1");
		   send(client_sockfd, buffer, strlen(buffer), 0);

		   recv(client_sockfd, buffer, BUFFER_SIZE, 0);
		   printf("Server says '%s'\n", buffer);

		   strcpy(buffer, "Message 2");
		   send(client_sockfd, buffer, strlen(buffer), 0);

		   recv(client_sockfd, buffer, BUFFER_SIZE, 0);
		   printf("Server says '%s'\n", buffer);

		   close(client_sockfd);

		 */

		HttpRequest* req = new_http_request();

		set_http_request_header(req, "Method", "GET");
		set_http_request_header(req, "Path", "http://localhost:xxxx/test.txt");
		set_http_request_header(req, "Version", "http/1.0");
		set_http_request_header(req, "Accept", "text/json");
		set_http_request_header(req, "Connection", "close");
		set_http_request_header(req, "User-Agent", "Mozilla/1.0");


		sprintf(req->body, "This is a request from a HTTP client\n");

		print_http_request(req);
		
		HttpResponse* res = make_http_request(req, PORT);

		print_http_response(res);		
		
		printf("Response : %s\n", res->body);


		return 0;
}
