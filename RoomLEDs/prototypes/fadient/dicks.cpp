// Example program
#include <iostream>
#include <cstring>
  
void printBits(size_t const size, void const * const ptr) {
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--) for (j=7;j>=0;j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
}

using namespace std;


int main() {
  srand(time(NULL));
  uint8_t arr[4];
  
  for (int8_t i=3; i>-1; i--) {
      arr[i]=rand();
      printf("%d ", arr[i]);
  }
  
  printf("\n");
  
  for (int8_t i=3; i>-1; i--) {
      printBits(sizeof(arr[0]), &arr[i]);
      printf(" ");
  }
  
  uint32_t* ptr = (uint32_t*)arr;
  printf("\n%u\n", *ptr);
  printBits(sizeof(ptr[0]), ptr);
  printf("\n");
}
