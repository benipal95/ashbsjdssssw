//  Tanvir Singh
//  TCSS 333
//  Assignment 3
#include <stdio.h>
#include <string.h>
#include "pixutils.h"

int main(int argc, char *argv[]){
  char *inputfile=0,*outputfile=0;
  unsigned char arrayType=0;
  float theta=0;
  int i=1;
  while (i<argc){
    if(!strcmp(argv[i],"-i")){
      inputfile=argv[i+1];
      i+=2;
    } else if(!strcmp(argv[i], "-o")) {
      outputfile = argv[i+1];
      i+=2;
    } else if(!strcmp(argv[i], "-r")) {
      theta = (float)atof(argv[i+1]);
      i+=2;
    } else if(!strcmp(argv[i], "-t")) {
      arrayType = (int)atoi((argv[i+1]));
      i+=2;
    } else{
      fprintf(stderr,"usage: frame -i <inputfile> -o <outputfile> -r <theta> -t <0-2>");
      exit(0);
    }
		}
  pixMap *p=pixMap_read(inputfile,arrayType);
  if(!p){
    fprintf(stderr,"unable to allocate memory for pixMap\n");
    return 1;
		}
  pixMap_rotate(p,theta);
  pixMap_write(p,outputfile);
  pixMap_destroy(&p);
  
  return 0;
}

