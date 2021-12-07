/* A simple ray tracer */
#include "vector_math.h"
#include "file_operation.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> /* Needed for boolean datatype */
#include <math.h>
#include <pthread.h> // For threads
#include <semaphore.h>

#define min(a, b)(((a) < (b)) ? (a) : (b))

int pixels = 0;

/* Width and height of out image */
#define WIDTH 800
#define HEIGHT 600
#define THREADS_NUM 600 // have to be  HEIGHT % THREADS_NUM == 0

pthread_mutex_t pixelcountMutex;

light lights[3];
material materials[3];
sphere spheres[3];

typedef struct{
  unsigned char img[3 * WIDTH * HEIGHT]; //will containe the raw image
} imageData;

typedef struct
{
  unsigned char* img;
  int* imagePosition;
} dataForThread;

void initImageData(){

  materials[0].diffuse.red = 1;
  materials[0].diffuse.green = 0;
  materials[0].diffuse.blue = 0;
  materials[0].reflection = 0.2;

  materials[1].diffuse.red = 0;
  materials[1].diffuse.green = 1;
  materials[1].diffuse.blue = 0;
  materials[1].reflection = 0.5;

  materials[2].diffuse.red = 0;
  materials[2].diffuse.green = 0;
  materials[2].diffuse.blue = 1;
  materials[2].reflection = 0.9;

  spheres[0].pos.x = 200;
  spheres[0].pos.y = 300;
  spheres[0].pos.z = 0;
  spheres[0].radius = 100;
  spheres[0].material = 0;

  spheres[1].pos.x = 400;
  spheres[1].pos.y = 400;
  spheres[1].pos.z = 0;
  spheres[1].radius = 100;
  spheres[1].material = 1;

  spheres[2].pos.x = 500;
  spheres[2].pos.y = 140;
  spheres[2].pos.z = 0;
  spheres[2].radius = 100;
  spheres[2].material = 2;

  lights[0].pos.x = 0;
  lights[0].pos.y = 240;
  lights[0].pos.z = -100;
  lights[0].intensity.red = 1;
  lights[0].intensity.green = 1;
  lights[0].intensity.blue = 1;

  lights[1].pos.x = 3200;
  lights[1].pos.y = 3000;
  lights[1].pos.z = -1000;
  lights[1].intensity.red = 0.6;
  lights[1].intensity.green = 0.7;
  lights[1].intensity.blue = 1;

  lights[2].pos.x = 600;
  lights[2].pos.y = 0;
  lights[2].pos.z = -100;
  lights[2].intensity.red = 0.3;
  lights[2].intensity.green = 0.5;
  lights[2].intensity.blue = 1;
}

void* threadDuty(void* arg){
  dataForThread* threadData = (dataForThread*) arg;

  printf("\033[1;35m Thread goes from  %d to %d in HEIGHT\n", *threadData->imagePosition, HEIGHT/THREADS_NUM + *threadData->imagePosition);
  
  ray r;
  int x, y;
  for (y = *threadData->imagePosition; y < HEIGHT/THREADS_NUM + *threadData->imagePosition; y++) {
    for (x = 0; x < WIDTH; x++) {

      float red = 0;
      float green = 0;
      float blue = 0;

      int level = 0;
      float coef = 1.0;

      r.start.x = x;
      r.start.y = y;
      r.start.z = -2000;

      r.dir.x = 0;
      r.dir.y = 0;
      r.dir.z = 1;

      do {
        /* Find closest intersection */
        float t = 20000.0f;
        int currentSphere = -1;

        unsigned int i;
        for (i = 0; i < 3; i++) { //here the intersection of the ray with shape is checked
          if (intersectRaySphere( &r, &spheres[i], &t))
            currentSphere = i; // if ray intersect the shape => return shape index 
        }
        if (currentSphere == -1) break; // brake condition ( if ray did not intersect the shape)

        vector scaled = vectorScale(t, &r.dir);
        vector newStart = vectorAdd( &r.start, &scaled);

        /* Find the normal for this new vector at the point of intersection */
        vector n = vectorSub( &newStart, &spheres[currentSphere].pos);
        float temp = vectorDot( &n, &n);
        if (temp == 0) break;

        temp = 1.0f / sqrtf(temp);
        n = vectorScale(temp, &n);

        /* Find the material to determine the colour */
        material currentMat = materials[spheres[currentSphere].material];

        /* Find the value of the light at this point */
        //lights[j]
        unsigned int j;
        for (j = 0; j < 3; j++) { //loop goes through all sourses of light
          light currentLight = lights[j];
          vector dist = vectorSub( &currentLight.pos, &newStart);
          if (vectorDot( &n, &dist) <= 0.0f) continue;
          float t = sqrtf(vectorDot( &dist, &dist));
          if (t <= 0.0f) continue;

          ray lightRay;
          lightRay.start = newStart;
          lightRay.dir = vectorScale((1 / t), &dist);

          /* Lambert diffusion */
          float lambert = vectorDot( &lightRay.dir, &n) * coef;
          red += lambert * currentLight.intensity.red * currentMat.diffuse.red;
          green += lambert * currentLight.intensity.green * currentMat.diffuse.green;
          blue += lambert * currentLight.intensity.blue * currentMat.diffuse.blue;
        }
        /* Iterate over the reflection */
        coef *= currentMat.reflection;

        /* The reflected ray start and direction */
        r.start = newStart;
        float reflect = 2.0f * vectorDot(&r.dir, &n);
        vector tmp = vectorScale(reflect, &n);
        r.dir = vectorSub( &r.dir, &tmp);

        level++;

      } while ((coef > 0.0f) && (level < 15)); //level it is a number of reflections of one ray

      
      threadData->img[(x + y * WIDTH) * 3 + 0] = (unsigned char) min(red * 255.0f, 255.0f);
      threadData->img[(x + y * WIDTH) * 3 + 1] = (unsigned char) min(green * 255.0f, 255.0f);
      threadData->img[(x + y * WIDTH) * 3 + 2] = (unsigned char) min(blue * 255.0f, 255.0f);

      // here the magic is happening  
      pthread_mutex_lock(&pixelcountMutex); // lock
      pixels++; 
      pthread_mutex_unlock(&pixelcountMutex);// unlock 
    }
  }
  free(threadData->imagePosition);
  free(threadData);
  return 0;
}

int main(int argc, char * argv[]) {
  //define array of threads
  pthread_t th[THREADS_NUM];
  
  //creating mutex
  pthread_mutex_init(&pixelcountMutex, NULL);

  initImageData();
  unsigned char img[3 * WIDTH * HEIGHT]; //will containe the raw image

  for (int i = 0; i < THREADS_NUM; i++)
  {
    dataForThread* threadData = malloc(sizeof(dataForThread));
    threadData->img = img;
    threadData->imagePosition = malloc(sizeof(int));
    *threadData->imagePosition = i * HEIGHT/THREADS_NUM;

    if (pthread_create(&th[i], NULL, &threadDuty, (void*)threadData) != 0)
      {
        perror("Failed to create thread");
      }
  }
  
  for (int i = 0; i < THREADS_NUM; i++)
  {
    if (pthread_join(th[i], NULL) != 0)
      {
        perror("Failed to join thread");
      }
  }

  //saving img to a file
  saveppm("image.ppm", img, WIDTH, HEIGHT);
  
  //destroying mutex  
  pthread_mutex_destroy(&pixelcountMutex);
  printf("\033[1;33m Total pixels rendered by threads: %d\n", pixels);
  printf("\033[0m");
  return 0;
}