/* Implementation of testing code for queue code */

#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> /* strcasecmp */
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "dudect/fixture.h"
#include "list.h"
#include "list_sort.h"

/* Our program needs to use regular malloc/free */
#define INTERNAL 1
#include "harness.h"

/* What character limit will be used for displaying strings? */
#define MAXSTRING 1024

/* How much padding should be added to check for string overrun? */
#define STRINGPAD MAXSTRING

/*
 * It is a bit sketchy to use this #include file on the solution version of the
 * code.
 * OK as long as head field of queue_t structure is in first position in
 * solution code
 */
#include "queue.h"
#include "queue_shuffle.h"

#include "console.h"
#include "report.h"

/* Settable parameters */

#define HISTORY_LEN 20

/*
 * How large is a queue before it's considered big.
 * This affects how it gets printed
 * and whether cautious mode is used when freeing the queue
 */
#define BIG_LIST 30
static int big_list_size = BIG_LIST;


/* Global variables */

/* List being tested */
typedef struct {
    struct list_head *l;
    /* meta data of list */
    int size;
} list_head_meta_t;

static list_head_meta_t l_meta;

/* Number of elements in queue */
static size_t lcnt = 0;

/* How many times can queue operations fail */
static int fail_limit = BIG_LIST;
static int fail_count = 0;

static int string_length = MAXSTRING;

/* original written sort: 0, lib/list_sort.c: 1 */
static int sortmode = 0;

#define MIN_RANDSTR_LEN 5
#define MAX_RANDSTR_LEN 10
static const char charset[] = "abcdefghijklmnopqrstuvwxyz";

/* Forward declarations */
static bool show_queue(int vlevel);

static bool do_free(int argc, char *argv[])
{
    return 0;
}

static bool do_new(int argc, char *argv[])
{
    return 0;
}

/*
 * TODO: Add a buf_size check of if the buf_size may be less
 * than MIN_RANDSTR_LEN.
 */
static void fill_rand_string(char *buf, size_t buf_size)
{
    
}

/* insert head */
static bool do_ih(int argc, char *argv[])
{
    return 0;
}

/* insert tail */
static bool do_it(int argc, char *argv[])
{
    return 0;
}

static bool do_remove(int option, int argc, char *argv[])
{
    return 0;
}

static inline bool do_rh(int argc, char *argv[])
{
    return 0;
}

static inline bool do_rt(int argc, char *argv[])
{
    return 0;
}

/* remove head quietly */
static bool do_rhq(int argc, char *argv[])
{
    return 0;
}

static bool do_dedup(int argc, char *argv[])
{
    return 0;
}

static bool do_reverse(int argc, char *argv[])
{
    return 0;
}

static bool do_size(int argc, char *argv[])
{
    return 0;
}

static int cmp(void *priv, const struct list_head *a, const struct list_head *b)
{
    return 0;
}

bool do_sort(int argc, char *argv[])
{
    return 0;
}

static bool do_dm(int argc, char *argv[])
{
    return 0;
}

static bool do_swap(int argc, char *argv[])
{
    return 0;
}

static bool is_circular()
{
    return 0;
}

static bool show_queue(int vlevel)
{
    return 0;
}

static bool do_show(int argc, char *argv[])
{
    return 0;
}

static bool do_shuffle(int argc, char *argv[])
{
    return 0;
}

static void console_init()
{
    ADD_COMMAND(new, "                | Create new queue");
    ADD_COMMAND(free, "                | Delete queue");
    ADD_COMMAND(
        ih,
        " str [n]        | Insert string str at head of queue n times. "
        "Generate random string(s) if str equals RAND. (default: n == 1)");
    ADD_COMMAND(
        it,
        " str [n]        | Insert string str at tail of queue n times. "
        "Generate random string(s) if str equals RAND. (default: n == 1)");
    ADD_COMMAND(
        rh,
        " [str]          | Remove from head of queue.  Optionally compare "
        "to expected value str");
    ADD_COMMAND(
        rt,
        " [str]          | Remove from tail of queue.  Optionally compare "
        "to expected value str");
    ADD_COMMAND(
        rhq,
        "                | Remove from head of queue without reporting value.");
    ADD_COMMAND(reverse, "                | Reverse queue");
    ADD_COMMAND(sort, "                | Sort queue in ascending order");
    ADD_COMMAND(
        size, " [n]            | Compute queue size n times (default: n == 1)");
    ADD_COMMAND(show, "                | Show queue contents");
    ADD_COMMAND(dm, "                | Delete middle node in queue");
    ADD_COMMAND(
        dedup, "                | Delete all nodes that have duplicate string");
    ADD_COMMAND(swap,
                "                | Swap every two adjacent nodes in queue");
    ADD_COMMAND(shuffle, "                | Shuffle nodes in queue");
    add_param("length", &string_length, "Maximum length of displayed string",
              NULL);
    add_param("malloc", &fail_probability, "Malloc failure probability percent",
              NULL);
    add_param("fail", &fail_limit,
              "Number of times allow queue operations to return false", NULL);
    add_param("sort", &sortmode,
              "Switch between lib/list_sort.c and the original written sort",
              NULL);
}

/* Signal handlers */
static void sigsegvhandler(int sig)
{
    report(1,
           "Segmentation fault occurred.  You dereferenced a NULL or invalid "
           "pointer");
    /* Raising a SIGABRT signal to produce a core dump for debugging. */
    abort();
}

static void sigalrmhandler(int sig)
{
    trigger_exception(
        "Time limit exceeded.  Either you are in an infinite loop, or your "
        "code is too inefficient");
}

static void queue_init()
{
    fail_count = 0;
    l_meta.l = NULL;
    signal(SIGSEGV, sigsegvhandler);
    signal(SIGALRM, sigalrmhandler);
}

static bool queue_quit(int argc, char *argv[])
{
    report(3, "Freeing queue");
    if (lcnt > big_list_size)
        set_cautious_mode(false);

    if (exception_setup(true))
        q_free(l_meta.l);
    exception_cancel();
    set_cautious_mode(true);

    size_t bcnt = allocation_check();
    if (bcnt > 0) {
        report(1, "ERROR: Freed queue, but %lu blocks are still allocated",
               bcnt);
        return false;
    }

    return true;
}

static void usage(char *cmd)
{
    printf("Usage: %s [-h] [-f IFILE][-v VLEVEL][-l LFILE]\n", cmd);
    printf("\t-h         Print this information\n");
    printf("\t-f IFILE   Read commands from IFILE\n");
    printf("\t-v VLEVEL  Set verbosity level\n");
    printf("\t-l LFILE   Echo results to LFILE\n");
    exit(0);
}

#define GIT_HOOK ".git/hooks/"
static bool sanity_check()
{
    struct stat buf;
    /* Directory .git not found */
    if (stat(".git", &buf)) {
        fprintf(stderr,
                "FATAL: You should run qtest in the directory containing valid "
                "git workspace.\n");
        return false;
    }
    /* Expected pre-commit and pre-push hooks not found */
    if (stat(GIT_HOOK "commit-msg", &buf) ||
        stat(GIT_HOOK "pre-commit", &buf) || stat(GIT_HOOK "pre-push", &buf)) {
        fprintf(stderr, "FATAL: Git hooks are not properly installed.\n");

        /* Attempt to install Git hooks */
        char *argv[] = {"sh", "-c", "scripts/install-git-hooks", NULL};
        extern char **environ;
        pid_t pid;
        int status = posix_spawn(&pid, "/bin/sh", NULL, NULL, argv, environ);
        if (status == 0) {
            /* Finally, succeed */
            if ((waitpid(pid, &status, 0) != -1) && (status == 0))
                return true;
            perror("waitpid");
        }
        return false;
    }
    return true;
}


#define BUFSIZE 256
int main(int argc, char *argv[])
{
    /* sanity check for git hook integration */
    if (!sanity_check())
        return -1;

    /* To hold input file name */
    char buf[BUFSIZE];
    char *infile_name = NULL;
    char lbuf[BUFSIZE];
    char *logfile_name = NULL;
    int level = 4;
    int c;

    while ((c = getopt(argc, argv, "hv:f:l:")) != -1) {
        switch (c) {
        case 'h':
            usage(argv[0]);
            break;
        case 'f':
            strncpy(buf, optarg, BUFSIZE);
            buf[BUFSIZE - 1] = '\0';
            infile_name = buf;
            break;
        case 'v': {
            char *endptr;
            errno = 0;
            level = strtol(optarg, &endptr, 10);
            if (errno != 0 || endptr == optarg) {
                fprintf(stderr, "Invalid verbosity level\n");
                exit(EXIT_FAILURE);
            }
            break;
        }
        case 'l':
            strncpy(lbuf, optarg, BUFSIZE);
            buf[BUFSIZE - 1] = '\0';
            logfile_name = lbuf;
            break;
        default:
            printf("Unknown option '%c'\n", c);
            usage(argv[0]);
            break;
        }
    }

    srand((unsigned int) (time(NULL)));
    queue_init();
    init_cmd();
    console_init();

    if (!infile_name) {
        /* Trigger call back function(auto completion) */
        linenoiseSetCompletionCallback(completion);
        linenoiseHistorySetMaxLen(HISTORY_LEN);
        linenoiseHistoryLoad(HISTORY_FILE); /* Load the history at startup */
    }
    set_verblevel(level);
    if (level > 1) {
        set_echo(true);
    }
    if (logfile_name)
        set_logfile(logfile_name);

    add_quit_helper(queue_quit);

    bool ok = true;
    ok = ok && run_console(infile_name);
    ok = finish_cmd() && ok;

    return ok ? 0 : 1;
}
