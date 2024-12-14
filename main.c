#include <stdlib.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define PATHMAX 256

#define HOME    getenv("HOME")
#define LIST    ".bash_todo/list"
#define TEMP    ".bash_todo/tmp"

#define HELP    (1 << 0)
#define ADD     (1 << 1)
#define READ    (1 << 2)
#define ERASE   (1 << 3)


static void path(char *path,
                 char *basename)
{
    const size_t maxlen = strlen(HOME) + strlen("/") + strlen(LIST) + 1;

    snprintf(path,
             maxlen,
             "%s/%s",
             HOME,
             basename);
}

static void print_help(void)
{
    printf("Simple to-do application for organising work and tasks.\n"
           "Options:\n"
           "  -h, --help    Display the help message\n");
}

static void add_task(const char *task)
{
    char fp[PATHMAX];
    path(fp,
         LIST);

    const size_t size = sizeof(char);
    const size_t n = strlen(task);

    FILE *file = fopen(fp,
                       "a");

    fwrite(task,
           size,
           n,
           file);

    putc('\n',
         file);

    fclose(file);
}

static void read_tasks(void)
{
    char fp[PATHMAX];
    path(fp,
         LIST);

    FILE *file = fopen(fp,
                       "r");

    char *line = NULL;
    size_t n;
    int i = 0;

    while ((getline(&line,
                    &n,
                    file)) != -1)
    {
        printf("[%d] %s",
               ++i,
               line);
    }

    free(line);
    fclose(file);
}

static void erase_task(const long index)
{
    char lp[PATHMAX];
    char tp[PATHMAX];

    path(lp,
         LIST);

    path(tp,
         TEMP);

    FILE *list = fopen(lp,
                       "r");

    FILE *temp = fopen(tp,
                       "w");

    char *line = NULL;
    const size_t size = sizeof(char);
    size_t n;
    int i = 0;

    while ((getline(&line,
                    &n,
                    list)) != -1)
    {
        if (++i == index)
        {
            continue;
        }

        n = strlen(line);

        fwrite(line,
               size,
               n,
               temp);
    }

    fclose(list);
    fclose(temp);

    remove(lp);
    rename(tp,
           lp);
}


int main(const int argc,
         char *const argv[])
{
    int opt;
    int option_index = 0;
    int args = 0;
    long index;
    const char *add_arg;

    static struct option long_options[] = {
        { "help",  no_argument,       0, 'h' },
        { "add",   required_argument, 0, 'a' },
        { "read",  no_argument,       0, 'r' },
        { "erase", required_argument, 0, 'e' },
        {  0,      0,                 0,  0  }
    };

    while ((opt = getopt_long(argc,
                              argv,
                              "ha:re:",
                              long_options,
                              &option_index)) != -1)
    {
        switch (opt)
        {
            case 'h':
                args = HELP;
                break;
            case 'a':
                args = ADD;
                add_arg = optarg;
                break;
            case 'r':
                args = READ;
                break;
            case 'e':
                args = ERASE;
                index = strtol(optarg,
                               NULL,
                               10);
            default:
                break;
        }
    }

    // TODO: if no args, indicate the error

    if (args & HELP)
    {
        print_help();
    }
    else if (args & ADD)
    {
        add_task(add_arg);
    }
    else if (args & READ)
    {
        read_tasks();
    }
    else if (args & ERASE)
    {
        erase_task(index);
    }

    return 0;
}
