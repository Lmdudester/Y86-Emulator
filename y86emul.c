#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "memory.h"

/*Main Method*/

int main(int argc, char ** argv){
  //If the number of arguments isn't two
  if(argc != 2){
    fprintf(stderr, "ERROR: Improper number of arguments.\nUse -h for help.\n");
    exit(1);
  }

  //If the user asks for help with the -h flag
  if(strcmp(argv[1],"-h") == 0){
    printf("Usage: y86emul <y86 input file>\n");
    exit(0);
  }

  //Ensures the file is a .y86 file
  char * input = strstr(argv[1],".y86");
  if(input == NULL || strlen(input) != 4 || strlen(argv[1]) < 5){
    fprintf(stderr, "ERROR: Improper file name.\n");
    exit(1);
  }

  //Opens file (for reading only)
  FILE * fp = fopen(argv[1], "r");
  if(fp == NULL){
      fprintf(stderr, "ERROR: File %s failed to open...\n", argv[1]);
      return 1;
  }

  char * line = NULL;
  size_t len = 0;
  ssize_t read = 0;
  while ((read = getline(&line, &len, fp)) != -1) { //gets a line

      //Divides the line by tabs
      char * token = strtok(line, "\t");
      while(token != NULL){

        if(strcmp(token,".size") == 0){ //.size
          token = strtok(NULL, "\n"); //ensures there isnt an extra '\n' at the end of a line

          memSize((int) strtol(token, NULL, 16));

        } else if(strcmp(token,".string") == 0){ // .string
          token = strtok(NULL, "\t");
          int addr = (int) strtol(token, NULL, 16);

          token = strtok(NULL, "\n"); //ensures there isnt an extra '\n' at the end of a line

          char * ptr = token + 1;

          while(*ptr != '\0' && *ptr != '\"'){
            mem[segCheck(addr)].byteChar = *ptr;
            ptr++;
            addr++;
          }

        } else if(strcmp(token,".long") == 0){ // .long
          //printf("Long Directive.\n");

          token = strtok(NULL, "\t");
          int addr = (int) strtol(token, NULL, 16);
          //printf("Hex Address: %s\n", token);

          token = strtok(NULL, "\n"); //ensures there isnt an extra '\n' at the end of a line
          int_Char val;
          val.intPart = (int) strtol(token, NULL, 16);

          mem[segCheck(addr)].byteChar = val.chars.b1;
          mem[segCheck(addr) + 1].byteChar = val.chars.b2;
          mem[segCheck(addr) + 2].byteChar = val.chars.b3;
          mem[segCheck(addr) + 3].byteChar = val.chars.b4;

        } else if(strcmp(token,".byte") == 0){ // .byte
          token = strtok(NULL, "\t");
          int addr = (int) strtol(token, NULL, 16);

          token = strtok(NULL, "\n"); //ensures there isnt an extra '\n' at the end of a line
          int val = (int) strtol(token, NULL, 16);
          mem[segCheck(addr)].byteChar = (char) val;

        } else if(strcmp(token,".text") == 0){ // .text
          token = strtok(NULL, "\t");
          int addr = (int) strtol(token, NULL, 16);

          ip = addr;

          token = strtok(NULL, "\n"); //ensures there isnt an extra '\n' at the end of a line

          //place in and convert String
          char * ptr = token;

          while(*ptr != '\0' && isspace(*ptr) == 0){ //*ptr != '\0'
            segCheck(addr);
            mem[addr].nibb.partOne = getVal(*ptr);
            ptr++;
            if(*ptr != '\0' && isspace(*ptr) == 0){
              mem[addr].nibb.partTwo = getVal(*ptr);
              ptr++;
            }
            addr++;
          }

        } else {
          //ERROR
          fprintf(stderr, "ERROR: %s is not a valid derective.\n", token);
        }

        token = strtok(NULL, "\t"); //STOPS INFINITE LOOP

      }

  }

  //Closes file, throw error if file close failure
  if(fclose(fp) != 0){
    fprintf(stderr, "ERROR: File exit failure.\n");
    exit(1);
  }

  //EXECUTE the program
  int command = 0;

  while(command != -1){
    command = mem[ip].nibb.partOne;

    switch(command){
      case 0: //nop
        increment(1);
        break;

      case 1: //halt
        command = -1;
        break;

      case 2: //rrmovl
        rrmovl();
        break;

      case 3: //irmovl
        irmovl();
        break;

      case 4: //rmmovl
        rmmovl();
        break;

      case 5: //mrmovl
        mrmovl();
        break;

      case 6: //op1
        opl();
        break;

      case 7: //jxx
        jcon();
        break;

      case 8: //call
        call();
        break;

      case 9: //ret
        ret();
        break;

      case 10: //pushl
        pushl();
        break;

      case 11: //popl
        popl();
        break;

      case 12: //readX
        readX();
        break;

      case 13: //writeX
        writeX();
        break;

      case 14: //movsbl
        movsbl();
        break;

      default:
        fprintf(stderr, "Error: INS - Invalid command 'F'...\n");
        exit(1);
    }
  }

  freeMem(); //free memory

  printf("Y86 Finished: AOK, HLT\n");
  return 0;
}
