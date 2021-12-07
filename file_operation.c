#include "file_operation.h"

/* Output data as PPMfile */
void saveppm(char* filename, unsigned char* img, int width, int height) {
  /* FILE pointer */
  FILE * f;

  /* Openfilefor writing */
  f = fopen(filename, "wb");

  /* PPM header info, including the size of the image */
  fprintf(f, "P6 %d %d %d\n", width, height, 255);

  /* Write the image data to thefile - remember 3 byte per pixel */
  fwrite(img, 3, width * height, f);

  /* Make sure you close thefile */
  fclose(f);
}
