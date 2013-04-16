#include <stdlib.h>
#include <string.h>

struct request{
	char* method;
	char* object;
	char* protocol;
	char* headers;
};

struct response{
	char* protocol;
	char* code;
	char* msg;
	char* headers;
	char* body;
};

// Request functions
struct request* requestFromBuffer(char* buf, int len);

/* Get header value
* Parameters:
*	req - Pointer on request structure
*	name - Name of header
* 
* Return:
*	pointer on header value string or null
*/
char* getRequestHeaderValue(struct request* req, char* name);

void freeRequest(struct request* req);

// Response functions
struct response* createResponse(char* protocol, char *code, char *msg);
void addHeaderResponse(struct response* resp, char* header);
void addBodyResponse(struct response* resp, char* body);
char* responseToBuffer(struct response* resp);
void freeResponse(struct response* resp);
/* Create page not found server response
* Return:
*	Pointer on new server response structure
*/
struct response* pageNotFoundResponse();

