#include <stdlib.h>
#include <stdio.h>

#ifndef _MEMORY_H_
#define _MEMORY_H_

//Registers
int reg[8];

//Flags
int of;
int zf;
int sf;

//Instruction Pointer
unsigned int ip;

//2 4-bit ints (representing the two Chars) to Byte
typedef union{
  struct{
    unsigned int partTwo : 4;
    unsigned int partOne : 4;
  } nibb;
  char byteChar;
} byte;

//32 bit Int to Chars
typedef union{
  struct{
    char b1, b2, b3, b4;
  } chars;
  int intPart;
} int_Char;

//Pointer to allocated memory
byte * mem;

//Size of Stack (Max Displacement) - In bytes
int totalSize;


/*Opening and Closing Memory*/

//Allocate memory as a char *
void memSize(unsigned int s){
  totalSize = s;
  mem = malloc(s*sizeof(byte));
  if(mem == NULL){
    fprintf(stderr, "ERROR: Failed to allocate memory.\n");
    exit(1);
  }
}

//Free memory when done
void freeMem(){
  free(mem);
}

/*Helper Methods*/

//Exits on an invalid register
int isRegister(unsigned int i){
    if(i > 7){
      fprintf(stderr, "ERROR: INS - %i is not a valid register. IP: %i", i, ip);
      exit(1);
    }
    return i;
}

//Increments Instruction Pointer Safely
void increment(unsigned int dis){
  ip += dis;
  if(ip >= totalSize){
    fprintf(stderr, "ERROR: ADR - Segmentation fault.IP: %i\n", ip);
    exit(1);
  }
}

int segCheck(int x){
  if(x < 0 || x >= totalSize){
    fprintf(stderr, "ERROR: ADR - Segmentation fault. Attempted Access to: %i\n", ip);
    exit(1);
  }
  return x;
}

//get int val
int getVal(char c){
  if(c >= '0' && c <= '9')
    return c - '0';
  if(c >= 'A' && c <= 'F')
    return 10 + c - 'A';
  if(c >= 'a' && c <= 'f')
    return 10 + c - 'a';
  fprintf(stderr, "ERROR: Invalid hex character: %c.\n", c);
  exit(1);
}

/*Y86 Instructions*/

//nop - 00 - 0 - does nothing

//halt - 10 - 16 - stops program - USE exit(0);

//rrmovl - 20 - 32 - moves register to Register
void rrmovl(){
  if(mem[ip].nibb.partTwo != 0){
    fprintf(stderr, "ERROR: INS - Second half of first byte is not 0 for rrmovl.\n");
    exit(1);
  }

  increment(1);
  isRegister(mem[ip].nibb.partOne);
  isRegister(mem[ip].nibb.partTwo);

  reg[mem[ip].nibb.partTwo] = reg[mem[ip].nibb.partOne];
  increment(1);
}

//irmovl - 30 - 48 - moves immediate value to register
void irmovl(){
  if(mem[ip].nibb.partTwo != 0){
    fprintf(stderr, "ERROR: INS - Second half of first byte is not 0 for irmovl.\n");
    exit(1);
  }

  increment(1);
  if(mem[ip].nibb.partOne != 15){
    fprintf(stderr, "ERROR: INS - irmovl Syntax - Register filler is not F. IP: %i\n", ip);
    exit(1);
  }

  unsigned int r = isRegister(mem[ip].nibb.partTwo);

  int_Char val;

  increment(1);
  val.chars.b1 = mem[ip].byteChar;
  increment(1);
  val.chars.b2 = mem[ip].byteChar;
  increment(1);
  val.chars.b3 = mem[ip].byteChar;
  increment(1);
  val.chars.b4 = mem[ip].byteChar;

  reg[r] = val.intPart;
  increment(1);
}

//rmmovl - 40 - 64 - moves value from register to memory
void rmmovl(){
  if(mem[ip].nibb.partTwo != 0){
    fprintf(stderr, "ERROR: INS - Second half of first byte is not 0 for rmmovl.\n");
    exit(1);
  }

  increment(1);

  unsigned int rA = isRegister(mem[ip].nibb.partOne);
  unsigned int rB = isRegister(mem[ip].nibb.partTwo);
  int_Char dis;

  increment(1);
  dis.chars.b1 = mem[ip].byteChar;
  increment(1);
  dis.chars.b2 = mem[ip].byteChar;
  increment(1);
  dis.chars.b3 = mem[ip].byteChar;
  increment(1);
  dis.chars.b4 = mem[ip].byteChar;

  dis.intPart = dis.intPart + reg[rB];

  if(dis.intPart < 0 || dis.intPart + 3 >= totalSize){
    fprintf(stderr, "ERROR: ADR - rmmovl Segmentation Fault. IP: %i\n", ip);
    exit(1);
  }

  int_Char out;
  out.intPart = reg[rA];

  mem[dis.intPart].byteChar = out.chars.b1;
  mem[dis.intPart + 1].byteChar = out.chars.b2;
  mem[dis.intPart + 2].byteChar = out.chars.b3;
  mem[dis.intPart + 3].byteChar = out.chars.b4;

  increment(1);
}

//mrmovl - 50 - 80 - moves mem to reg
void mrmovl(){
  if(mem[ip].nibb.partTwo != 0){
    fprintf(stderr, "ERROR: INS - Second half of first byte is not 0 for mrmovl.\n");
    exit(1);
  }

  increment(1);

  unsigned int rA = isRegister(mem[ip].nibb.partOne);
  unsigned int rB = isRegister(mem[ip].nibb.partTwo);
  int_Char dis;

  increment(1);
  dis.chars.b1 = mem[ip].byteChar;
  increment(1);
  dis.chars.b2 = mem[ip].byteChar;
  increment(1);
  dis.chars.b3 = mem[ip].byteChar;
  increment(1);
  dis.chars.b4 = mem[ip].byteChar;

  dis.intPart = dis.intPart + reg[rB];

  if(dis.intPart < 0 || dis.intPart + 3 >= totalSize){
    fprintf(stderr, "ERROR: ADR - mrmovl Segmentation Fault. IP: %i\n", ip);
    exit(1);
  }

  int_Char out;

  out.chars.b1 = mem[dis.intPart].byteChar;
  out.chars.b2 = mem[dis.intPart + 1].byteChar;
  out.chars.b3 = mem[dis.intPart + 2].byteChar;
  out.chars.b4 = mem[dis.intPart + 3].byteChar;

  reg[rA] = out.intPart;

  increment(1);
}

/*opl Helpers*/
void addl(){ //0
  int rA = reg[mem[ip].nibb.partOne];
  int rB = reg[mem[ip].nibb.partTwo];

  if(rA > 0 && rB > 0 && rA + rB < 0)
    of = 1;
  else if(rA < 0 && rB < 0 && rA + rB > 0)
    of = 1;
  else
    of = 0;

  reg[mem[ip].nibb.partTwo] = rB + rA;

  if(reg[mem[ip].nibb.partTwo] == 0)
    zf = 1;
  else
    zf = 0;

  if(reg[mem[ip].nibb.partTwo] < 0)
    sf = 1;
  else
    sf = 0;
}

void subl(){ //1
  int rA = reg[mem[ip].nibb.partOne];
  int rB = reg[mem[ip].nibb.partTwo];

  if(rA < 0 && rB > 0 && rA - rB > 0)
    of = 1;
  else if(rA > 0 && rB < 0 && rA - rB < 0)
    of = 1;
  else
    of = 0;

  reg[mem[ip].nibb.partTwo] = rB - rA;

  if(reg[mem[ip].nibb.partTwo] == 0)
    zf = 1;
  else
    zf = 0;

  if(reg[mem[ip].nibb.partTwo] < 0)
    sf = 1;
  else
    sf = 0;
}

void andl(){ //2
  int newVal = reg[mem[ip].nibb.partOne] & reg[mem[ip].nibb.partTwo];

  of = 0;

  if(newVal == 0)
    zf = 1;
  else
    zf = 0;

  if(newVal < 0)
    sf = 1;
  else
    sf = 0;
}

void xorl(){ //3
  int newVal = reg[mem[ip].nibb.partOne] ^ reg[mem[ip].nibb.partTwo];

  of = 0;

  if(newVal == 0)
    zf = 1;
  else
    zf = 0;

  if(newVal < 0)
    sf = 1;
  else
    sf = 0;
}

void mull(){ //4
  int rA = reg[mem[ip].nibb.partOne];
  int rB = reg[mem[ip].nibb.partTwo];

  if(rA > 0 && rB > 0 && rA * rB < 0)
    of = 1;
  else if(rA < 0 && rB < 0 && rA * rB < 0)
    of = 1;
  else if(rA > 0 && rB < 0 && rA * rB > 0)
    of = 1;
  else if(rA < 0 && rB > 0 && rA * rB > 0)
    of = 1;
  else
    of = 0;

  reg[mem[ip].nibb.partTwo] = rB * rA;

  if(reg[mem[ip].nibb.partTwo] == 0)
    zf = 1;
  else
    zf = 0;

  if(reg[mem[ip].nibb.partTwo] < 0)
    sf = 1;
  else
    sf = 0;
}

void cmpl(){ //5
  int rA = reg[mem[ip].nibb.partOne];
  int rB = reg[mem[ip].nibb.partTwo];

  of = 0;

  int res =  rB - rA;

  if(res == 0)
    zf = 1;
  else
    zf = 0;

  if(res < 0)
    sf = 1;
  else
    sf = 0;
}

//opl - 60_65 - 96_101 - performs op and stores in rA
void opl(){
  int op = mem[ip].nibb.partTwo;
  increment(1);

  switch(op){
    case 0:
      addl();
      break;

    case 1:
      subl();
      break;

    case 2:
      andl();
      break;

    case 3:
      xorl();
      break;

    case 4:
      mull();
      break;

    case 5:
      cmpl();
      break;

    default:
      fprintf(stderr, "ERROR: INS - Improper operation type for opl. IP: %i\n", ip);
      exit(1);
  }

  increment(1);
}

/*jcon helper*/

void jmp() {
  int_Char dest;

  dest.chars.b1 = mem[ip].byteChar;
  increment(1);
  dest.chars.b2 = mem[ip].byteChar;
  increment(1);
  dest.chars.b3 = mem[ip].byteChar;
  increment(1);
  dest.chars.b4 = mem[ip].byteChar;

  if(dest.intPart >= totalSize || dest.intPart < 0){
    fprintf(stderr, "ERROR: ADR - jmp Segmentation Fault. IP: %i\n", ip);
    exit(1);
  }

  ip = dest.intPart;
}

//jcon - 70_76 - 112_118 - moves ip to new location
void jcon(){
  int op = mem[ip].nibb.partTwo;
  increment(1);

  switch(op){
    case 0: //jmp
      jmp();
      break;

    case 1: //jle
      if((sf ^ of) == 1 || zf == 1)
        jmp();
      else
        increment(4);
      break;

    case 2: //jl
      if((sf ^ of) == 1)
        jmp();
      else
        increment(4);
      break;

    case 3: //je
      if(zf == 1)
        jmp();
      else
        increment(4);
      break;

    case 4: //jne
      if(zf == 0)
        jmp();
      else
        increment(4);
      break;

    case 5: //jge
      if((sf ^ of) == 0)
        jmp();
      else
        increment(4);
      break;

    case 6: //jg
      if((sf ^ of) == 0 && zf == 0)
        jmp();
      else
        increment(4);
      break;

    default:
      fprintf(stderr, "ERROR: INS - Improper operation type for jump. IP: %i\n", ip);
      exit(1);
  }

}

void call(){ //80 - 128 - calls a function
  if(mem[ip].nibb.partTwo != 0){
    fprintf(stderr, "ERROR: INS - Second half of first byte is not 0 for call.\n");
    exit(1);
  }

  increment(1);

  //Store dest
  int_Char dest;

  dest.chars.b1 = mem[ip].byteChar;
  increment(1);
  dest.chars.b2 = mem[ip].byteChar;
  increment(1);
  dest.chars.b3 = mem[ip].byteChar;
  increment(1);
  dest.chars.b4 = mem[ip].byteChar;
  increment(1);

  //Push ip
  int_Char retAdd;
  retAdd.intPart = ip;

  if(reg[4] - 4 <= 0) {
    fprintf(stderr, "ERROR: ADR - Segmentation Fault for esp. IP: %i\n", ip);
    exit(1);
  }

  mem[reg[4] - 4].byteChar = retAdd.chars.b1;
  mem[reg[4] - 3].byteChar = retAdd.chars.b2;
  mem[reg[4] - 2].byteChar = retAdd.chars.b3;
  mem[reg[4] - 1].byteChar = retAdd.chars.b4;

  reg[4] -= 4;

  //printf("dest: %d\t", dest.intPart);

  if(dest.intPart < 0 || dest.intPart >= totalSize){
    fprintf(stderr, "ERROR: ADR - Segmentation Fault for dest in call. IP: %i\n", ip);
    exit(1);
  }

  ip = dest.intPart;
}

void ret(){ //90 - 144 - pops, copies ret address into ip,
  if(mem[ip].nibb.partTwo != 0){
    fprintf(stderr, "ERROR: INS - Second half of first byte is not 0 for ret.\n");
    exit(1);
  }

  if(reg[4] + 3 >= totalSize) {
    fprintf(stderr, "ERROR: ADR - Segmentation Fault for esp. IP: %i\n", ip);
    exit(1);
  }

  int_Char retAdd;

  retAdd.chars.b4 = mem[reg[4] + 3].byteChar;
  retAdd.chars.b3 = mem[reg[4] + 2].byteChar;
  retAdd.chars.b2 = mem[reg[4] + 1].byteChar;
  retAdd.chars.b1 = mem[reg[4]].byteChar;

  if(retAdd.intPart < 0 || retAdd.intPart >= totalSize){
    fprintf(stderr, "ERROR: ADR - Segmentation Fault for return address %i in ret. IP: %i\n", retAdd.intPart, ip);
    exit(1);
  }

  reg[4] += 4;

  ip = retAdd.intPart;
}

void pushl(){ //A0 - 160 - pushes rA onto the stack
  if(mem[ip].nibb.partTwo != 0){
    fprintf(stderr, "ERROR: INS - Second half of first byte is not 0 for pushl.\n");
    exit(1);
  }

  increment(1);
  isRegister(mem[ip].nibb.partOne);

  if(mem[ip].nibb.partTwo != 15){
    fprintf(stderr, "ERROR: INS - pushl Syntax - Register filler is not F. IP: %i\n", ip);
    exit(1);
  }

  if(reg[4] - 4 <= 0) {
    fprintf(stderr, "ERROR: ADR - Segmentation Fault for esp. IP: %i\n", ip);
    exit(1);
  }

  int_Char retAdd;
  retAdd.intPart = reg[mem[ip].nibb.partOne];

  //may need to be reversed
  mem[reg[4] - 4].byteChar = retAdd.chars.b1;
  mem[reg[4] - 3].byteChar = retAdd.chars.b2;
  mem[reg[4] - 2].byteChar = retAdd.chars.b3;
  mem[reg[4] - 1].byteChar = retAdd.chars.b4;

  reg[4] -= 4;

  increment(1);
}

void popl(){ //B0 - 176 - pops from stack into rA
  if(mem[ip].nibb.partTwo != 0){
    fprintf(stderr, "ERROR: INS - Second half of first byte is not 0 for popl.\n");
    exit(1);
  }

  increment(1);
  isRegister(mem[ip].nibb.partOne);

  if(mem[ip].nibb.partTwo != 15){
    fprintf(stderr, "ERROR: INS - pushl Syntax - Register filler is not F. IP: %i\n", ip);
    exit(1);
  }

  if(reg[4] + 3 >= totalSize) {
    fprintf(stderr, "ERROR: ADR - Segmentation Fault for esp. IP: %i\n", ip);
    exit(1);
  }

  int_Char forReg;

  forReg.chars.b1 = mem[reg[4]].byteChar;
  forReg.chars.b2 = mem[reg[4] + 1].byteChar;
  forReg.chars.b3 = mem[reg[4] + 2].byteChar;
  forReg.chars.b4 = mem[reg[4] + 3].byteChar;

  reg[4] += 4;

  reg[mem[ip].nibb.partOne] = forReg.intPart;
  increment(1);
}

void readX(){ //C0_C1 192_193 - reads a character or an integer and stores it
  int op = mem[ip].nibb.partTwo;

  if(op < 0 || op > 1){
    fprintf(stderr, "ERROR: INS - Improper function type for readX. IP: %i\n", ip);
    exit(1);
  }

  increment(1);

  if(mem[ip].nibb.partTwo != 15){
    fprintf(stderr, "ERROR: INS - readX Syntax - Register filler is not F. IP: %i\n", ip);
    exit(1);
  }

  isRegister(mem[ip].nibb.partOne);
  int dest = reg[mem[ip].nibb.partOne];

  int_Char disp;

  increment(1);
  disp.chars.b1 = mem[ip].byteChar;
  increment(1);
  disp.chars.b2 = mem[ip].byteChar;
  increment(1);
  disp.chars.b3 = mem[ip].byteChar;
  increment(1);
  disp.chars.b4 = mem[ip].byteChar;
  increment(1);

  dest += disp.intPart;

  if(dest < 0 || dest >= totalSize){
    fprintf(stderr, "ERROR: ADR - Segmentation Fault for dest %i in readX. IP: %i\n", dest, ip);
    exit(1);
  }

  if(op == 0){ //char
    mem[dest].byteChar = getchar();

    if(mem[dest].byteChar == -1)
      zf = 1;
    else
      zf = 0;

  } else { //int
    if(dest + 4 < 0 || dest + 4 >= totalSize){
      fprintf(stderr, "ERROR: ADR - Segmentation Fault for dest %i in readX. IP: %i\n", dest + 4, ip);
      exit(1);
    }

    if(EOF == scanf("%d", &disp.intPart))
      zf = 1;
    else
      zf = 0;

    mem[dest + 3].byteChar = disp.chars.b4;
    mem[dest + 2].byteChar = disp.chars.b3;
    mem[dest + 1].byteChar = disp.chars.b2;
    mem[dest].byteChar = disp.chars.b1;
  }
}

void writeX(){ //D0_D1 - N/A - reads a character or an integer from mem and prints
  int op = mem[ip].nibb.partTwo;

  if(op < 0 || op > 1){
    fprintf(stderr, "ERROR: INS - Improper function type for writeX. IP: %i\n", ip);
    exit(1);
  }

  increment(1);

  if(mem[ip].nibb.partTwo != 15){
    fprintf(stderr, "ERROR: INS - writeX Syntax - Register filler is not F. IP: %i\n", ip);
    exit(1);
  }

  isRegister(mem[ip].nibb.partOne);
  int dest = reg[mem[ip].nibb.partOne];

  int_Char disp;

  increment(1);
  disp.chars.b1 = mem[ip].byteChar;
  increment(1);
  disp.chars.b2 = mem[ip].byteChar;
  increment(1);
  disp.chars.b3 = mem[ip].byteChar;
  increment(1);
  disp.chars.b4 = mem[ip].byteChar;
  increment(1);

  dest += disp.intPart;

  if(dest < 0 || dest >= totalSize){
    fprintf(stderr, "ERROR: ADR - Segmentation Fault for dest %i in writeX. IP: %i\n", dest, ip);
    exit(1);
  }

  if(op == 0){ //Char
    printf("%c", mem[dest].byteChar);

  } else { //Int
    if(dest + 4 < 0 || dest + 4 >= totalSize){
      fprintf(stderr, "ERROR: ADR - Segmentation Fault for dest %i in writeX. IP: %i\n", dest + 4, ip);
      exit(1);
    }

    disp.chars.b4 = mem[dest + 3].byteChar;
    disp.chars.b3 = mem[dest + 2].byteChar;
    disp.chars.b2 = mem[dest + 1].byteChar;
    disp.chars.b1 = mem[dest].byteChar;

    printf("%d", disp.intPart);
  }
}

void movsbl(){ //E0 - N/A - takes a byte and extends it into a long
  if(mem[ip].nibb.partTwo != 0){
    fprintf(stderr, "ERROR: INS - Second half of first byte is not 0 in movsbl. IP: %i\n", ip);
    exit(1);
  }

  increment(1);

  isRegister(mem[ip].nibb.partOne);
  isRegister(mem[ip].nibb.partTwo);

  int storageReg = mem[ip].nibb.partOne;
  int dest = reg[mem[ip].nibb.partTwo];

  int_Char disp;

  increment(1);
  disp.chars.b1 = mem[ip].byteChar;
  increment(1);
  disp.chars.b2 = mem[ip].byteChar;
  increment(1);
  disp.chars.b3 = mem[ip].byteChar;
  increment(1);
  disp.chars.b4 = mem[ip].byteChar;
  increment(1);

  dest += disp.intPart;

  if(dest < 0 || dest >= totalSize){
    fprintf(stderr, "ERROR: ADR - Segmentation Fault for dest %i in movsbl. IP: %i\n", dest, ip);
    exit(1);
  }

  if(mem[dest].nibb.partOne >= 8){ //Extend sign
    disp.intPart = -1;
    disp.chars.b1 = mem[dest].byteChar;

  } else { //Do nothing
    disp.intPart = 0;
    disp.chars.b1 = mem[dest].byteChar;
  }

  reg[storageReg] = disp.intPart;
}

#endif /* _TEST_H_ */
