#include <stdio.h>
#include <stdlib.h>


#if 1

void* _malloc(size_t size, const char *file, int line){

  void *p = malloc(size);
  
  printf("malloc[+]\t %p:%s:%d\n", p, file, line);
  
//  char buff[128] = {0};
//	sprintf(buff, "./mem/%p.mem", p);
//	FILE *fp = fopen(buff, "w");
//	fprintf(fp, "[+%s:%d] --> addr: %p, size: %ld\n", file, line, p, size)
//
//	fflush(fp);
//	fclose(fp);
  return p;
}

void _free(void* p, const char *file, int line){
  free(p);
  printf("free[-]\t\t %p:%s:%d\n", p, file, line);  
}

#define malloc(size) _malloc(size, __FILE__, __LINE__)
#define free(p) _free(p, __FILE__, __LINE__)

#endif


int main(int argc, char **argv){

  void* p = malloc(10);
  
  free(p);

  return 0;
}