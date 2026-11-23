#include <stdio.h>
#include "lappy_stdlib.h"

int main(void) {
  stdio_init();

  while (1) {
    printf("Hello World!!\n");
    delay(1000);
  }
}