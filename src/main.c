#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAPESZ 32768

static const char *instr_set = "+-<>[],.";

void
file_seek_to(FILE *fp, char sentinel)
{
    int ch;

    while((ch = fgetc(fp)) != EOF)
        if(ch == sentinel)
            return;
}

static ssize_t
file_read_line(char *buffer, size_t bufsz, FILE *fp)
{
    int ch;
    char *bufp = buffer;

    while((ch = fgetc(fp)) != EOF){
        if(ch == '\n')
            break;

        /* ignore the rest of the line*/
        if((size_t)(bufp - buffer) == bufsz - 1){
            file_seek_to(fp, '\n');
            break;
        }

        *bufp++ = ch;
    }

    if(ch == EOF)
        return -1;

    *bufp = 0;
    return (ssize_t)(bufp - buffer);
}

static void
seek_to_closing_bracket(FILE *f)
{
    int ch;
    int depth = 0;

    while((ch = fgetc(f)) != EOF){
        if(ch == '[')
            ++depth;

        if(ch == ']'){
            --depth;

            if(depth == -1)
                return;
        }
    }
}

static int
minify(FILE *f)
{
    int ch;
    int first_write = 0;
    int last_ch = 0;

    if(f == NULL)
        return 0;

    while((ch = fgetc(f)) != EOF){
        /* strip non-bf characters */
        if(strchr("+-><[],.", ch) == NULL)
            continue;

        /* strip useless loops (immediately follows another loop) */
        if(last_ch == ']' && ch == '[' ){
            seek_to_closing_bracket(f);
            continue;
        }

        if(first_write == 0){
            /* strip useless loops (cells haven't been modified) */
            if(ch == '['){
                seek_to_closing_bracket(f);
                continue;
            }

            if(strchr("+-,", ch) != NULL)
                first_write = 1;
        }

        putchar(ch);
        last_ch = ch;
    }

    return 1;
}

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
