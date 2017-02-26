//  Tanvir Singh
//  TCSS 333
//  Assignment 3

#include "pixutils.h"

static pixMap* pixMap_init(unsigned char arrayType);
static pixMap* pixMap_copy(pixMap *p);
static pixMap* pixMap_init(unsigned char arrayType){
  
  //initialize everything to zero except arrayType
  pixMap *pix = (pixMap*) malloc(sizeof(pixMap));
  pix->arrayType = arrayType;
  pix->image = 0;
  pix->imageHeight = 0;
  pix->imageWidth = 0;
  pix->pixArray_arrays = 0;
  pix->pixArray_blocks = 0;
  pix->pixArray_overlay = 0;
  return pix;
}

void pixMap_destroy (pixMap **p){
  //free all mallocs and put a zero pointer in *p
  free(*p);
  *p = 0;
}

pixMap *pixMap_read(char *filename,unsigned char arrayType){
  //library call reads in the image into p->image and sets the width and height
  pixMap *p=pixMap_init(arrayType);
  int error;
  if((error=lodepng_decode32_file(&(p->image), &(p->imageWidth),
                                  &(p->imageHeight),filename))){
    fprintf(stderr,"error %u: %s\n", error, lodepng_error_text(error));
    return 0;
  }
  //allocate the 2-D rgba arrays
  
  if (arrayType ==0){
    if(p->imageWidth > MAXWIDTH) {
      printf("Error, image width is more than max width\n");
      return 0;
    }
    unsigned char *temp = p->image;
    p->pixArray_arrays = malloc(sizeof(rgba[MAXWIDTH])* p->imageHeight);
    for (int i = 0; i < p->imageHeight; i++) {
      memcpy(p->pixArray_arrays[i], temp, sizeof(rgba) * p->imageWidth);
      temp+= p->imageWidth * sizeof(rgba);
    }
  }
  else if (arrayType ==1){
    rgba **bl = (rgba**) malloc (sizeof(rgba*) * p->imageHeight);
    //use a loop allocate a block of memory for each row
    unsigned char *temp = p->image;
    for (int i = 0; i < p->imageHeight; i++) {
      *bl = (rgba*) malloc(sizeof(rgba) * p->imageWidth);
      memcpy(*bl, temp, sizeof(rgba)*p->imageWidth);
      temp+=p->imageWidth*sizeof(rgba);
      bl++;
    }
    bl-=p->imageHeight;
    p->pixArray_blocks = bl;
  }
  else if (arrayType ==2){
    unsigned char *temp = p->image;
    p->pixArray_overlay = (rgba**) malloc(sizeof(rgba*)*p->imageHeight);
    rgba **e = p->pixArray_overlay;
    for(int i = 0; i < p->imageHeight; i++) {
      *(e) = (rgba*) temp;
      e++;
      temp += sizeof(rgba)*p->imageWidth;
    }
  } else {
    return 0;
  }
  return p;
}
int pixMap_write(pixMap *p,char *filename){
  int error=0;
  if (p->arrayType ==0){
    //have to copy each row of the array into the corresponding row of the image
    unsigned char *temp = p->image;
    for (int i = 0; i < p->imageHeight; i++) {
      memcpy(temp, p->pixArray_arrays[i], sizeof(rgba) * p->imageWidth);
      temp+= p->imageWidth * sizeof(rgba);
    }
  }
  else if (p->arrayType ==1){
    //have to copy each row of the array into the corresponding row of the image
    unsigned char *temp = p->image;
    rgba **e = p->pixArray_blocks;
    for (int i = 0; i < p->imageHeight; i++) {
      memcpy(temp, *e, sizeof(rgba)*p->imageWidth);
      temp+=p->imageWidth * sizeof(rgba);
      e++;
    }
  }
  //library call to write the image out
  if(lodepng_encode32_file(filename, p->image, p->imageWidth,
                           p->imageHeight)){
    fprintf(stderr,"error %u: %s\n", error, lodepng_error_text(error));
    return 1;
  }
  return 0;
}

int pixMap_rotate(pixMap *p,float theta){
  pixMap *oldPixMap=pixMap_copy(p);
  if(!oldPixMap)return 1;
  
  const float ox=p->imageWidth/2.0f;
  const float oy=p->imageHeight/2.0f;
  const float s=sin(degreesToRadians(-theta));
  const float c=cos(degreesToRadians(-theta));
  for(int y=0;y<p->imageHeight;y++){
    for(int x=0;x<p->imageWidth;x++){
      float rotx = c*(x-ox) - s * (oy-y) + ox;
      float roty = -(s*(x-ox) + c * (oy-y) - oy);
      int rotj=rotx+.5;
      int roti=roty+.5;
      if(roti >=0 && roti < oldPixMap->imageHeight && rotj >=0 && rotj < oldPixMap->imageWidth){
        if(p->arrayType==0) memcpy(p->pixArray_arrays[y]+x,oldPixMap->pixArray_arrays[roti]+rotj,sizeof(rgba));
        else if(p->arrayType==1) memcpy(p->pixArray_blocks[y]+x,oldPixMap->pixArray_blocks[roti]+rotj,sizeof(rgba));
        else if(p->arrayType==2) memcpy(p->pixArray_overlay[y]+x,oldPixMap->pixArray_overlay[roti]+rotj,sizeof(rgba));
      }
      else{
        if(p->arrayType==0) memset(p->pixArray_arrays[y]+x,0,sizeof(rgba));
        else if(p->arrayType==1) memset(p->pixArray_blocks[y]+x,0,sizeof(rgba));
        else if(p->arrayType==2) memset(p->pixArray_overlay[y]+x,0,sizeof(rgba));
      }
    }
  }
  pixMap_destroy(&oldPixMap);
  
  return 0;
}

pixMap *pixMap_copy(pixMap *p){
  pixMap *new=pixMap_init(p->arrayType);
  //allocate memory for new image of the same size a p->image
  unsigned int size = p->imageHeight * p->imageWidth*sizeof(rgba);
  new->image = (unsigned char*) malloc (sizeof(unsigned char)*size);
  memcpy(new->image, p->image, size);
  new->imageHeight = p->imageHeight;
  new->imageWidth = p->imageWidth;
  //allocate memory and copy the arrays.
  if (new->arrayType ==0){
    new->pixArray_arrays =
    malloc(sizeof(rgba[MAXWIDTH])*new->imageHeight);
    for (int i = 0; i < p->imageHeight; i++) {
      memcpy(new->pixArray_arrays[i], p->pixArray_arrays[i],
             sizeof(rgba)*p->imageWidth);
    }
  }
  else if (new->arrayType == 1){
    rgba **bl = (rgba**) malloc (sizeof(rgba*)*p->imageHeight);
    rgba **temp = p->pixArray_blocks;
    for (int i = 0; i < p->imageHeight; i++) {
      *bl = (rgba*) malloc(sizeof(rgba)*p->imageWidth);
      memcpy(*bl, *temp, sizeof(rgba)*p->imageWidth);
      temp++;
      bl++;
    }
    bl-=p->imageHeight;
    new->pixArray_blocks = bl;
  }
  else if (new->arrayType ==2){
    unsigned char *temp = new->image;
    new->pixArray_overlay = (rgba**) malloc(sizeof(rgba*)
                                            *new->imageHeight);
    rgba **c = new->pixArray_overlay;
    for(int i = 0; i < new->imageHeight; i++) {
      *(c) = (rgba*) temp;
      c++;
      temp+= sizeof(rgba) * new->imageWidth;
    }
  }
  return new;
}
