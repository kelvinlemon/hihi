/*****************************************************************************/
// gcc -O3-o test_mmm_block test_mmm_block.c -lrt

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define GIG 1000000000

#define MLENGTH 2048
#define BITERS 7 
#define BBASE  16


#define OPTIONS 1

typedef double data_t;

/* Create abstract data type for matrix */
typedef struct {
  long int len;
  data_t *data;
} matrix_rec, *matrix_ptr;

//struct timespec {
//  time_t tv_sec; /* seconds */
//  long tv_nsec;  /* nanoseconds */
//};

/*****************************************************************************/
int main(int argc, char *argv[])
{
  int OPTION;
  struct timespec diff(struct timespec start, struct timespec end);
  struct timespec time1, time2;
  struct timespec time_stamp[OPTIONS][BITERS+1];
  int clock_gettime(clockid_t clk_id, struct timespec *tp);
  matrix_ptr new_matrix(long int len);
  int set_matrix_length(matrix_ptr m, long int index);
  long int get_matrix_length(matrix_ptr m);
  int init_matrix(matrix_ptr m, long int len);
  int zero_matrix(matrix_ptr m, long int len);
  void mmm_iijjkk_blocked(matrix_ptr a, matrix_ptr b, matrix_ptr c, long int block_size);

  long int i, j, k;
  long int bsize = BBASE;

  printf("\n Hello World -- MMM \n");

  // declare and initialize the matrix structure
  matrix_ptr a0 = new_matrix(MLENGTH);
  init_matrix(a0, MLENGTH);
  matrix_ptr b0 = new_matrix(MLENGTH);
  init_matrix(b0, MLENGTH);
  matrix_ptr c0 = new_matrix(MLENGTH);
  zero_matrix(c0, MLENGTH);

  OPTION = 0;

  for (i = 0; i < BITERS; i++) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    mmm_iijjkk_blocked(a0,b0,c0,bsize);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    time_stamp[OPTION][i] = diff(time1,time2);
    printf("\niter = %ld bsize = %ld", i, bsize);
    bsize *= 2;
  }

  bsize = BBASE;
  for (i = 0; i < BITERS; i++) {
    printf("\n%ld, %ld", bsize, (long int)((double)
					  (GIG * time_stamp[0][i].tv_sec + time_stamp[0][i].tv_nsec)));
    bsize *= 2;
  }

  printf("\n");
  
}/* end main */

/**********************************************/

/* Create matrix of specified length */
matrix_ptr new_matrix(long int len)
{
  long int i;

  /* Allocate and declare header structure */
  matrix_ptr result = (matrix_ptr) malloc(sizeof(matrix_rec));
  if (!result) return NULL;  /* Couldn't allocate storage */
  result->len = len;

  /* Allocate and declare array */
  if (len > 0) {
    data_t *data = (data_t *) calloc(len*len, sizeof(data_t));
    if (!data) {
	  free((void *) result);
	  printf("\n COULDN'T ALLOCATE STORAGE %ld\n", result->len);
	  return NULL;  /* Couldn't allocate storage */
	}
	result->data = data;
  }
  else result->data = NULL;

  return result;
}

/* Set length of matrix */
int set_matrix_length(matrix_ptr m, long int index)
{
  m->len = index;
  return 1;
}

/* Return length of matrix */
long int get_matrix_length(matrix_ptr m)
{
  return m->len;
}

/* initialize matrix */
int init_matrix(matrix_ptr m, long int len)
{
  long int i;

  if (len > 0) {
    m->len = len;
    for (i = 0; i < len*len; i++)
      m->data[i] = (data_t)(i);
    return 1;
  }
  else return 0;
}

/* initialize matrix */
int zero_matrix(matrix_ptr m, long int len)
{
  long int i,j;

  if (len > 0) {
    m->len = len;
    for (i = 0; i < len*len; i++)
      m->data[i] = (data_t)(0.0);
    return 1;
  }
  else return 0;
}

data_t *get_matrix_start(matrix_ptr m)
{
  return m->data;
}

/*************************************************/

struct timespec diff(struct timespec start, struct timespec end)
{
  struct timespec temp;
  if ((end.tv_nsec-start.tv_nsec)<0) {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return temp;
}

/*************************************************/

/* mmm */
void mmm_iijjkk_blocked(matrix_ptr a, matrix_ptr b, matrix_ptr c, long int block_size)
{
  long int i, j, k, ii, kk, jj;
  long int length = get_matrix_length(a);
  data_t *a0 = get_matrix_start(a);
  data_t *b0 = get_matrix_start(b);
  data_t *c0 = get_matrix_start(c);
  data_t sum;

  for (i = 0; i < length; i++)
    for (j = 0; j < length; j++) 
      c0[i*length+j] = 0.0;
  
  for (ii = 0; ii < length; ii += block_size)   
    for (jj = 0; jj < length; jj += block_size)
      for (kk = 0; kk < length; kk += block_size)
        for (i = ii; i < ii+block_size; i++)
          for (j = jj; j < jj+block_size; j++) {
            sum = c0[i*length+j];
            for (k = kk; k < kk+block_size; k++)
              sum +=  a0[i*length+k]*b0[k*length+j];
            c0[i*length+j] = sum;
          }
}
