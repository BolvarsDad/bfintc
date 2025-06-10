#include <stdio.h>
#include <stdlib.h>

#define TAPESZ 65536

static const char *instr_set = "+-<>[],.";

static size_t
get_line(char *buffer, size_t bufsz, char *filename)
{
    static char ch;
    size_t nread;
    size_t buflen = 0;

    FILE *file = fopen(filename, "r");

    if (file == NULL || feof(file))
        goto exit_eof;

    if (ferror(file))
        goto exit_ferror;

    while (buflen < bufsz) {
        if ((nread = fread(&ch, sizeof ch, 1, file)) == 0 || ch == '\n')
            break;

        buffer[buflen++] = ch;
    }

    return buflen;

exit_ferror:
    {
        fprintf(stderr, "get_line_implementation: Encountered I/O error.\n");
        return 0;
    }

exit_eof:
    return 0;
}

static void
build_lookup_table(char const *code, int *table)
{
    int stack[TAPESZ];
    int top = -1;

    for (int i = 0; code[i]; ++i) {
        if (code[i] == '[')
            stack[++top] = i;

        else if (code[i] == ']') {
            if (top < 0)
                goto exit_mismatch;

            int start = stack[top--];

            table[start] = i;
            table[i] = start;
        }
    }

    if (top >= 0)
        goto exit_mismatch;

    return;

exit_mismatch:
    {
        fprintf(stderr, "build_lookup_table(): Encountered mismatching brackets.\n");
        exit(1);
    }
}

static void
interpret(char const *code)
{
    unsigned char tape[TAPESZ] = {0};
    int jmptable[TAPESZ] = {0};
    int dataptr = 0;
    int instrptr = 0;

    build_lookup_table(code, jmptable);

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
                      if (tape[dataptr] == 0)
                          instrptr = jmptable[instrptr];
                      break;

            case ']':
                      if (tape[dataptr] != 0)
                          instrptr = jmptable[instrptr];
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

    char code[TAPESZ];
    size_t nread;

    nread = get_line(code, TAPESZ, argv[1]);

    printf("%.*s %lu\n", (int)nread, code, nread);

    interpret(code);

    return 0;
}
