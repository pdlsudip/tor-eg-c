#include <arpa/inet.h>
#include <dlfcn.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
#define PROXY "127.0.0.1"
#define PROXYPORT 9050

struct ProxyRequest {
  int8 vn;
  int8 cd;
  int16 dstport;
  int32 dstip;
  unsigned char userid[8];
};
struct ProxyResponse {
  int8 vn;
  int8 cd;
  int16 _;
  int32 __;
};
#define reqsize sizeof(struct ProxyRequest)
#define ressize sizeof(struct ProxyResponse)
#define USERNAME "sensei1"
struct ProxyRequest *request(struct sockaddr_in *);
int connect(int, const struct sockaddr *, socklen_t);
