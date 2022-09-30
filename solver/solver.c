#include <err.h>
#include <stdio.h>

int
main(int argc, char const *argv[])
{
  if (argc != 1) errx(1, " ");
  printf("oy\n");
  return 0;
}