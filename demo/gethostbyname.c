#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv){
  extern int h_errno;
  struct hostent *host;
  struct in_addr in;
  struct sockaddr_in addr_in;
  host = gethostbyname(argv[1]);
  if(host==NULL){
    perror("gethostbyname");
    printf("errno: %s\n", hstrerror(h_errno));
    exit(1);
  }
  for (int i=0;host->h_addr_list[i]!=NULL;i++){
    memcpy(&addr_in.sin_addr.s_addr, host->h_addr_list[i],4);
    //memcpy(&addr_in.sin_addr.s_addr, host->h_addr,4); //#define h_addr h_addr_list[0] /* for backward compatibility */
    in.s_addr=addr_in.sin_addr.s_addr;
    printf("ip:%s\n",inet_ntoa(in));
  }
  printf("host name:%s, ip lenght:%d, type:%d\n",host->h_name,host->h_length,host->h_addrtype);
  for (int i=0;host->h_aliases[i]!=NULL;i++){
    printf("%s\n",host->h_aliases[i]);
  }
  
  return 0;
}
