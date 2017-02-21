#include <stdlib.h>

#ifndef _STACK_H_
#define _STACK_H_

//Registers
extern int[8] reg;

//Flags
extern int of;
extern int zf;
extern int sf

//Instruction Pointer
extern int ip;

//Pointer to allocated memory
extern char * mem;

char * size(int size){
  mem = malloc(size);
  if(mem == NULL){
    
  }
}

void freeMem(){
  free(mem);
}

#endif /* _STACK_H_ */
