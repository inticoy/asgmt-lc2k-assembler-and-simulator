/* Assembler code fragment for LC-2K */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAXLINELENGTH 1000
#define NUMMEMORY 65536

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

int main(int argc, char *argv[])
{
  char *inFileString, *outFileString;
  FILE *inFilePtr, *outFilePtr;
  char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
  char label_name[10][NUMMEMORY];
  int label_address[NUMMEMORY];
  int label_i = 0;
  int label_total = 0;
  int current_address = 0;
  int bit_opcode, bit_0, bit_1, bit_2, instruction;
  int undefine = 1;

  if (argc != 3) {
    printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
    argv[0]);
    exit(1);
  }

  inFileString = argv[1];
  outFileString = argv[2];
  inFilePtr = fopen(inFileString, "r");

  if (inFilePtr == NULL) {
    printf("error in opening %s\n", inFileString);
    exit(1);
  }

  outFilePtr = fopen(outFileString, "w");
  // if (outFilePtr == NULL) {
  //   printf("error in opening %s\n", outFileString);
  //   exit(1);
  // }
  while(1){
    if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
      /* reached end of file */
      break;
    }
    if (strlen(label)) {
      strcpy(label_name[label_i], label);
      label_address[label_i] = current_address;
      label_i++;
    }
    current_address++;
  }

  rewind(inFilePtr);
  label_total = label_i + 1;
  current_address = 0;

  /*  exception duplicated labels */
  printf("\n");
  for(int i = 0; i < label_total; i++) {
    for(int j = i + 1; j < label_total; j++) {
      if (!strcmp(label_name[i], label_name[j])) {
        printf("error: duplicated labels\n");
        printf("duplicated label: %s\n", label_name[i]);
        exit(1);
      }
    }
  }


  while(1){
    if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
      /* reached end of file */
      break;
    }
    if (!strcmp(opcode, "add")) {
      /* do whatever you need to do for opcode "add" */
      bit_opcode = 0;
      bit_0 = (int)(*arg0 - '0');
      bit_1 = (int)(*arg1 - '0');
      bit_2 = (int)(*arg2 - '0');
      bit_opcode = bit_opcode << 22;
      bit_0 = bit_0 << 19;
      bit_1 = bit_1 << 16;
      instruction = bit_opcode | bit_0 | bit_1 | bit_2;
      fprintf(outFilePtr, "%d\n", instruction);
    } else if (!strcmp(opcode, "nor")) {
      bit_opcode = 1;
      bit_0 = (int)(*arg0 - '0');
      bit_1 = (int)(*arg1 - '0');
      bit_2 = (int)(*arg2 - '0');
      bit_opcode = bit_opcode << 22;
      bit_0 = bit_0 << 19;
      bit_1 = bit_1 << 16;
      instruction = bit_opcode | bit_0 | bit_1 | bit_2;
      fprintf(outFilePtr, "%d\n", instruction);
    } else if (!strcmp(opcode, "lw")) {
      bit_opcode = 2;
      bit_0 = (int)(*arg0 - '0');
      bit_1 = (int)(*arg1 - '0');
      if(isNumber(arg2)){
        bit_2 = (int)(*arg2 - '0');
      } else {
        undefine = 1;
        for(int i = 0; i < label_total; i++){
          if(!strcmp(label_name[i], arg2)){
            undefine = 0;
            bit_2 = *(label_address + i);
            break;
          }
        }
        /* undefined labels */
        if (undefine) {
          printf("error: undefined labels\n");
          printf("undefined label: %s\n", arg2);
          exit(1);
        }
      }
      /* offsetField that don't fit in 16 bits */
      if ((bit_2 > 32767) || (bit_2 < -32768)){
        printf("error: offset field don't fit in 16 bits\n");
        exit(1);
      }


      bit_opcode = bit_opcode << 22;
      bit_0 = bit_0 << 19;
      bit_1 = bit_1 << 16;
      instruction = bit_opcode | bit_0 | bit_1 | bit_2;
      fprintf(outFilePtr, "%d\n", instruction);
    } else if (!strcmp(opcode, "sw")) {
      bit_opcode = 3;
      bit_0 = (int)(*arg0 - '0');
      bit_1 = (int)(*arg1 - '0');
      if(isNumber(arg2)){
        if (strlen(arg2) == 1) {
          bit_2 = (int)(*arg2 - '0');
        } else {
          bit_2 = atoi(arg2);
        }
      } else {
        undefine = 1;
        for(int i = 0; i < label_total; i++){
          if(!strcmp(label_name[i], arg2)){
            undefine = 0;
            bit_2 = *(label_address + i);
            break;
          }
        }
        /* undefined labels */
        if (undefine) {
          printf("error: undefined labels\n");
          printf("undefined label: %s\n", arg2);
          exit(1);
        }
      }

      /* offsetField that don't fit in 16 bits */
      if ((bit_2 > 32767) || (bit_2 < -32768)){
        printf("error: offset field don't fit in 16 bits\n");
        exit(1);
      }

      bit_opcode = bit_opcode << 22;
      bit_0 = bit_0 << 19;
      bit_1 = bit_1 << 16;

      instruction = bit_opcode | bit_0 | bit_1 | bit_2;
      fprintf(outFilePtr, "%d\n", instruction);
    } else if (!strcmp(opcode, "beq")) {
      bit_opcode = 4;
      bit_0 = (int)(*arg0 - '0');
      bit_1 = (int)(*arg1 - '0');
      if(isNumber(arg2)){
        bit_2 = (int)(*arg2 - '0');
      } else {
        undefine = 1;
        for(int i = 0; i < label_total; i++){
          if(!strcmp(label_name[i], arg2)){
            undefine = 0;
            bit_2 = *(label_address + i) - current_address - 1;
            break;
          }
        }
        if (undefine) {
          printf("error: undefined labels\n");
          printf("undefined label: %s\n", arg2);
          exit(1);
        }
      }

      /* offsetField that don't fit in 16 bits */
      if ((bit_2 > 32767) || (bit_2 < -32768)){
        printf("error: offset field don't fit in 16 bits\n");
        exit(1);
      }

      bit_opcode = bit_opcode << 22;
      bit_0 = bit_0 << 19;
      bit_1 = bit_1 << 16;
      int mask = 0xFFFF;
      bit_2 = bit_2 & mask;

      instruction = bit_opcode | bit_0 | bit_1 | bit_2;
      fprintf(outFilePtr, "%d\n", instruction);
    } else if (!strcmp(opcode, "jalr")) {
      bit_opcode = 5;
      bit_0 = (int)(*arg0 - '0');
      bit_1 = (int)(*arg1 - '0');
      bit_opcode = bit_opcode << 22;
      bit_0 = bit_0 << 19;
      bit_1 = bit_1 << 16;
      instruction = bit_opcode | bit_0 | bit_1;
      fprintf(outFilePtr, "%d\n", instruction);
    } else if (!strcmp(opcode, "halt")) {
      bit_opcode = 6;
      bit_opcode = bit_opcode << 22;
      fprintf(outFilePtr, "%d\n", bit_opcode);
    } else if (!strcmp(opcode, "noop")) {
      bit_opcode = 7;
      bit_opcode = bit_opcode << 22;
      fprintf(outFilePtr, "%d\n", bit_opcode);
    } else if (!strcmp(opcode, ".fill")) {
      if(isNumber(arg0)){
        fprintf(outFilePtr, "%s\n", arg0);
      } else {
        for(int i = 0; i < label_total; i++){
          if(!strcmp(label_name[i], arg0)){
            fprintf(outFilePtr, "%d\n", *(label_address + i));
            break;
          }
        }
      }
    } else {
      printf("error: unrecognized opcode\n");
      printf("%s\n", opcode);
      exit(1);
    }
    current_address++;
  }

  fclose(outFilePtr);
  return(0);
}

int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0, char *arg1, char *arg2)
{
  char line[MAXLINELENGTH];
  char *ptr = line;
  /* delete prior values */
  label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
  /* read the line from the assembly-language file */
  if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
  /* reached end of file */
  return(0);
  }
  /* check for line too long (by looking for a \n) */
  if (strchr(line, '\n') == NULL) {
  /* line too long */
  printf("error: line too long\n");
  exit(1);
  }
  /* is there a label? */
  ptr = line;
  if (sscanf(ptr, "%[^\t\n\r ]", label)) {
  /* successfully read label; advance pointer over the label */
  ptr += strlen(label);
  }
  /*
  * Parse the rest of the line. Would be nice to have real regular
  * expressions, but scanf will suffice. */
  sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
  return(1);
}

int isNumber(char *string)
{ /* return 1 if string is a number */
  int i;
  return( (sscanf(string, "%d", &i)) == 1);
}
