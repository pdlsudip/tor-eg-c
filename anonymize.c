#include "anonymize.h"

struct ProxyRequest *request(struct sockaddr_in *sock2) {
  struct ProxyRequest *req;
  req = malloc(reqsize);
  req->vn = 4;
  req->cd = 1;
  req->dstip = (sock2->sin_addr.s_addr);
  req->dstport = (sock2->sin_port);
  memset(req->userid, 0, sizeof(req->userid));
  strncpy((char *)req->userid, USERNAME, 7);
  return req;
}
int connect(int s2, const struct sockaddr *sock2, socklen_t len) {
  char buffer[ressize];
  struct sockaddr_in sock;
  sock.sin_family = AF_INET;
  sock.sin_port = htons(PROXYPORT);
  sock.sin_addr.s_addr = inet_addr(PROXY);

  struct ProxyRequest *req;
  struct ProxyResponse *res;
  int (*p)(int, const struct sockaddr *, socklen_t);

  p = dlsym(RTLD_NEXT, "connect");
  if (!p) {
    fprintf(stderr, "Error finding original connect: %s\n", dlerror());
    return -1;
  }

  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("socket");
    return -1;
  }

  if (p(s, (struct sockaddr *)&sock, sizeof(sock)) < 0) {
    perror("connect");
    close(s);
    return -1;
  }

  printf("Connected to Proxy\n");
  req = request((struct sockaddr_in *)sock2);
  if (!req) {
    close(s);
    return -1;
  }

  if (write(s, req, reqsize) < reqsize) {
    perror("write");
    free(req);
    close(s);
    return -1;
  }
  printf("Sent req");
  memset(buffer, 0, ressize);

  int total_read = 0, bytes;
  while (total_read < ressize) {
    bytes = read(s, buffer + total_read, ressize - total_read);
    if (bytes <= 0) {
      perror("read");
      free(req);
      close(s);
      return -1;
    }
    total_read += bytes;
  }

  res = (struct ProxyResponse *)buffer;
  printf("Proxy response: vn=%d, cd=%d\n", res->vn, res->cd);

  if (res->cd != 90) {
    fprintf(stderr, "Proxy error code: %d\n", res->cd);
    free(req);
    close(s);
    return -1;
  }

  // Send HTTP request

  char host[50] = "https://hianime.to/";
  char http_req[250];
  snprintf(http_req, sizeof(http_req),
           "GET / HTTP/1.0\r\n"
           "Host: %s\r\n"
           "\r\n",
           host);
  write(s, http_req, strlen(http_req));

  // Read response
  while ((bytes = read(s, buffer, sizeof(buffer) - 1)) > 0) {
    buffer[bytes] = '\0';
    printf("%s", buffer);
  }

  printf("Connected via proxy\n");

  close(s2);
  dup2(s, s2);
  free(req);
  close(s);
  return 0;
}
