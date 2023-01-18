//
//
//

#include <stdio.h>   // for printf()
#include <unistd.h>  // for getopt()

int main(int argc, char **argv) {
  int i;

  printf("argc=%d\n", argc); /* debugging */

  for (i = 0; i < argc; i++) {
    puts(argv[i]);
  }

  return 0;
} // end main()