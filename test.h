#include <stdlib.h>
#include <stdio.h>
#include "memory.h"

#ifndef _TEST_H_
#define _TEST_H_

//Get char val
char getChar(int i){
  if(i >= 0 && i <= 9)
    return '0' + i;
  if(i >= 10 && i <= 15)
    return 'A' + i - 10;
  return '\0';
}

//Copy string into mem
void copier(char * ptr){
  int size = 0;
  while(*ptr != '\0'){
    mem[size].nibb.partOne = getVal(*ptr);
    ptr++;
    if(*ptr == '\0'){
      printf("You fucked up.\n");
      exit(1);
    }
    mem[size].nibb.partTwo = getVal(*ptr);
    ptr++;
    size++;
  }
}

//Init Memory to 0
void initMem(){
  int size = 0;
  for(;size < totalSize; size++){
    mem[size].byteChar = 0;
  }
}

//Prints memory
void printmem(){
  int v = 0;
  int i = 1;
  printf("Memory:\n");
  for(; v < totalSize; v++){
    printf("%c", getChar(mem[v].nibb.partOne));
    printf("%c", getChar(mem[v].nibb.partTwo));

    if((v + 1) % 10 == 0){
      printf("\n %i0 ->", i);
      i++;
    }
  }
  printf("\n");
}

//Tests rrmovl & irmovl
void part1(){
  int size = 20;
  memSize(size);
  initMem();
  copier("30F0010000002001");

  ip = 0;

  printmem();

  irmovl();
  rrmovl();

  printf("reg[0]: %i, reg[1]: %i\n", reg[0], reg[1]);
}

//Tests rmmovl & mrmovl
void part2(){
  int size = 40;
  memSize(size);
  initMem();
  ip = 0;
  copier("400105000000500305000000");

  reg[0] = 1;
  reg[1] = 10;

  rmmovl();

  reg[0] = 0;
  reg[3] = 10;

  mrmovl();

  printmem();

  printf("Reg[0] = %i\n", reg[0]);

}

//Tests opl - WORKS AND SETS FLAGS
void part3(){
  int size = 20;
  memSize(size);
  initMem();
  ip = 0;
  reg[1] = 1;
  reg[0] = 0;
  copier("60016401");
  opl();
  opl();

  if(reg[0] == reg[1] && reg[0] == 0 && zf == 1)
    printf("opl works!\n");
  else{
    printf("opl failure...\n");
    printf("Reg 0: %i\n", reg[0]);
    printf("Reg 1: %i\n", reg[1]);
    printf("ZF: %i\n", zf);
    printf("OF: %i\n", of);
    printf("SF: %i\n", sf);
  }
}

void part4(){ //Tested jcon - works perfectly
  int size = 20;
  memSize(size);
  initMem();

  reg[0] = 2;
  reg[1] = 3; //reg1 CON_OP reg0

  copier("65017601000000"); //6501 - cmpl reg0 reg1

  ip = 0;

  opl();
  jcon();

  if(ip == 1)
    printf("jcon works!\n");
  else{
    printf("jcon failure...\nIP: %i\n", ip);
  }

}

void part5(){ //tests call
  int size = 20;
  memSize(size);
  initMem();
  copier("8001000000");
  reg[4] = 18;

  ip = 0;

  call();

  //printf("IP: %i\n", ip);

  //printmem();

  if(ip == 1)
    printf("call works!\n");
  else
    printf("call failure...\n");
}

void part6(){ //tests call and ret
  int size = 40;
  memSize(size);
  initMem();
  copier("800A000000000000000090000000000000000000");
  reg[4] = 39;

  ip = 0;

  call();
  ret();

  //printf("IP: %i\nesp: %i\n", ip, reg[4]);

  if(ip == 5)
    printf("ret works!\n");
  else
    printf("ret failure...\n");

  //printmem();
}

void part7(){ //tests push and pop
  int size = 20;
  memSize(size);
  initMem();
  copier("A00FB00F");
  reg[4] = 19;
  reg[0] = 20;

  ip = 0;

  pushl();
  reg[0] = 1;
  popl();

  if(reg[0] == 20)
    printf("push & pop succcess!\n");
  else
    printf("push or pop failure...\n");
}

void part8(){ //tests readX & writeX
  int size = 40;
  memSize(size);
  initMem();
  copier("C00F01000000C10F0A000000D00F01000000D10F0A000000");

  reg[0] = 20;

  ip = 0;

  printf("\nEnter a character: ");
  readX();
  printf("Enter a number: ");
  readX();

  int_Char out;

  out.chars.b1 = mem[30 + 3].byteChar;
  out.chars.b2 = mem[30 + 2].byteChar;
  out.chars.b3 = mem[30 + 1].byteChar;
  out.chars.b4 = mem[30].byteChar;

  printf("Character: %c\nInt: %d\n", mem[21].byteChar, out.intPart);
}

void part9(){
  int size = 20;
  memSize(size);
  initMem();
  copier("D0010000000000");

  printmem();

  reg[0] = -1;
  reg[1] = 10;
  ip = 0;

  movsbl();

  printf("Reg[0] = %i\n", reg[0]);

}

void tester(){
  //part1();
  part2();
  //part3();
  //part4();
  //part5();
  //part6();
  //part7();
  //part8();

  //part9();
}


#endif
