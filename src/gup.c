/*
 * Copyright (c) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "gup/state.h"
#include "gup/parser.h"

#define GUP_VERSION "0.0.1"

/* Output file path */
static const char *out_path = "a.out";

static void
help(void)
{
    printf(
        "gup compiler - gup bup wup!\n"
        "---------------------------\n"
        "[-h]   Display this help menu\n"
        "[-v]   Display the gup version\n"
        "[-o]   Output file path\n"
    );
}

static void
version(void)
{
    printf(
        "-------------------------------\n"
        "Copyright (c) 2026, Ian Moffett\n"
        "gup v%s\n"
        "-------------------------------\n",
        GUP_VERSION
    );
}

static void
compile(const char *path)
{
    struct gup_state state;

    if (path == NULL) {
        return;
    }

    if (gup_state_init(&state, path, out_path) < 0) {
        printf("fatal: failed to initialize gup state\n");
        perror("gup_state_init");
        return;
    }

    /* Pass 0 */
    if (gup_parse(&state) < 0) {
        gup_state_destroy(&state);
        return;
    }

    /* Pass 1 */
    if (gup_parse(&state) < 0) {
        gup_state_destroy(&state);
        return;
    }

    gup_state_destroy(&state);
}

int
main(int argc, char **argv)
{
    int opt;

    if (argc < 2) {
        printf("fatal: too few arguments\n");
        help();
        return -1;
    }

    while ((opt = getopt(argc, argv, "hvo:")) != -1) {
        switch (opt) {
        case 'h':
            help();
            return -1;
        case 'v':
            version();
            return -1;
        case 'o':
            out_path = strdup(optarg);
            break;
        }
    }

    while (optind < argc) {
        compile(argv[optind++]);
    }

    return 0;
}
