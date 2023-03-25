#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if 0

void* _malloc(size_t size, const char *file, int line){

  void *p = malloc(size);
  
//  printf("malloc[+]\t %p:%s:%d\n", p, file, line);
  
  char buff[128] = {0};
	sprintf(buff, "./mem/%p.mem", p);
	FILE *fp = fopen(buff, "w");
	fprintf(fp, "[+%s:%d] --> addr: %p, size: %ld\n", file, line, p, size);

	fflush(fp);
	fclose(fp);
  return p;
}

void _free(void* p, const char *file, int line){
  
  char buff[128] = {0};
	sprintf(buff, "./mem/%p.mem", p);
  if (unlink(buff)<0){
    printf("double free: %p\n", p);
    return;  
  }
  free(p);
//  printf("free[-]\t\t %p:%s:%d\n", p, file, line);  
}

#define malloc(size) _malloc(size, __FILE__, __LINE__)
#define free(p) _free(p, __FILE__, __LINE__)

#else
typedef void* (*malloc_t)(size_t size);
malloc_t malloc_f = NULL;

typedef void (*free_t)(void *p);
free_t free_f;

static void init_hook(void){
  if (malloc_f == NULL)
    malloc_f = dlsym(RTLD_NEXT, "malloc");
  
  if (free_f == NULL)
    free_f = dlsym(RTLD_NEXT, "free");

}

#define DEBUG_MEM_LEAK  init_hook();

#endif


int main(int argc, char **argv){
  DEBUG_MEM_LEAK;

  void* p = malloc(10);
  
  free(p);

  return 0;
}