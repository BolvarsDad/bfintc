#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAPESZ 32768

static const char *instr_set = "+-<>[],.";

static void
interpret(char const *code)
{
    unsigned char tape[TAPESZ] = {0};
    int dataptr = 0;
    int instrptr = 0;

    while (code[instrptr] != '\0')
    {
        switch (code[instrptr])
        {
            case '+': ++tape[dataptr]; break;
            case '-': --tape[dataptr]; break;
            case '>': ++dataptr; break;
            case '<': --dataptr; break;
            case '.': putchar(tape[dataptr]); break;
            case ',': tape[dataptr] = getchar(); break;
            case '[':
                      if (tape[dataptr] == 0) {
                          int depth = 0;

                          while (++instrptr) {
                              if (code[instrptr] == '[')
                                  depth += 1;
                              if (code[instrptr] == ']') {
                                  if (depth == 0)
                                      break;

                                  depth -= 1;
                              }
                          }
                      }
                      break;
            case ']':
                      if (tape[dataptr] != 0) {
                          int depth = 0;

                          while (--instrptr) {
                              if (code[instrptr] == ']')
                                  depth += 1;
                              if (code[instrptr] == '[') {
                                  if (depth == 0)
                                      break;

                                  depth -= 1;
                              }
                          }
                      }
                      break;
            default:
                      break;
        }
        instrptr++;
    }
}

static void
usage(char const *progname)
{
    printf("Usage: %s file.bf\n", progname);
    exit(1);
}

int
main(int argc, char **argv)
{
    if (argc < 2)
        usage(argv[0]);

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
        return 1;

    char code[TAPESZ] = {0};
    size_t len = fread(code, 1, TAPESZ - 1, fp);

    code[len] = '\0';
    fclose(fp);

    interpret(code);

    return 0;
}
