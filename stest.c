/**
 * @file stest.c
 * @author Matheus T. dos Santos (matheuswhite1@protonmail.com)
 * @brief
 * @version 0.1
 * @date 01/11/24
 *
 * @copyright Copyright (c) 2024 Citrinio All rights reserved.
 */
#include "stest.h"

#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

jmp_buf buf;

extern struct ssuite start_ssuite_region __asm("section$start$__DATA$__ssuite_region");
extern struct ssuite end_ssuite_region __asm("section$end$__DATA$__ssuite_region");

extern struct stest start_stest_region __asm("section$start$__DATA$__stest_region");
extern struct stest end_stest_region __asm("section$end$__DATA$__stest_region");

static void signal_handler(int sigNum) { longjmp(buf, sigNum); }

static const struct ssuite *get_suite(const struct ssuite *suites, size_t suites_size, const char *name) {
    for (size_t i = 0; i < suites_size; i++) {
        if (strcmp(suites[i].name, name) == 0) {
            return &suites[i];
        }
    }

    return NULL;
}

static const struct stest *get_test(const struct ssuite *suites, size_t suites_size, const struct stest *tests,
                                    size_t tests_size, const char *suite_name, const char *test_name) {
    for (size_t i = 0; i < suites_size; i++) {
        if (strcmp(suites[i].name, suite_name) == 0) {
            for (size_t j = 0; j < tests_size; j++) {
                if (strcmp(tests[j].name, test_name) == 0) {
                    return &tests[j];
                }
            }
        }
    }

    return NULL;
}

static void handle_arguments(int argc, char *argv[], char **suite_name, char **test_name) {
    if (argc < 2) {
        return;
    }

    if (strcmp(argv[1], "-s") == 0) {
        *suite_name = argv[2];
    } else if (strcmp(argv[1], "-t") == 0) {
        *test_name = argv[2];
    }

    if (argc < 4) {
        return;
    }

    if (strcmp(argv[3], "-s") == 0) {
        *suite_name = argv[4];
    } else if (strcmp(argv[3], "-t") == 0) {
        *test_name = argv[4];
    }
}

static int run_all_tests(void) {
    const struct stest *stest_region = &start_stest_region;
    size_t stest_region_size = &end_stest_region - &start_stest_region;

    for (size_t i = 0; i < stest_region_size; i++) {
        int err = setjmp(buf);

        if (err != 0) {
            printf("\tFailed with signal %d\n", err);
            continue;
        }

        printf("[%s:%s] Running...\n", stest_region[i].ssuite->name, stest_region[i].name);
        err = stest_region[i].fn();
        if (err == 0) {
            printf("\tSuccess\n");
        } else {
            printf("\tFailed with error: %d\n", err);
        }
    }


    return 0;
}

static int run_all_suite_tests(char *suite_name) {
    const struct ssuite *ssuite_region = &start_ssuite_region;
    const struct stest *stest_region = &start_stest_region;
    size_t ssuite_region_size = &end_ssuite_region - &start_ssuite_region;
    size_t stest_region_size = &end_stest_region - &start_stest_region;

    const struct ssuite *suite = get_suite(ssuite_region, ssuite_region_size, suite_name);
    if (suite == NULL) {
        printf("There is no test suite with name: %s\n", suite_name);
        return -ENOENT;
    }

    for (size_t i = 0; i < stest_region_size; i++) {
        if (stest_region[i].ssuite != suite) {
            continue;
        }

        int err = setjmp(buf);

        if (err != 0) {
            printf("\tFailed with signal %d\n", err);
            continue;
        }

        printf("[%s:%s] Running...\n", stest_region[i].ssuite->name, stest_region[i].name);
        err = stest_region[i].fn();
        if (err == 0) {
            printf("\tSuccess\n");
        } else {
            printf("\tFailed with error: %d\n", err);
        }
    }

    return 0;
}

static int run_single_test(char *suite_name, char *test_name) {
    const struct ssuite *ssuite_region = &start_ssuite_region;
    const struct stest *stest_region = &start_stest_region;
    size_t ssuite_region_size = &end_ssuite_region - &start_ssuite_region;
    size_t stest_region_size = &end_stest_region - &start_stest_region;

    const struct stest *test =
            get_test(ssuite_region, ssuite_region_size, stest_region, stest_region_size, suite_name, test_name);
    if (test == NULL) {
        printf("There is no test with name %s inside %s\n", test_name, suite_name);
        return -ENOENT;
    }

    int err = setjmp(buf);

    if (err != 0) {
        printf("\tFailed with signal %d\n", err);
        return 0;
    }

    printf("[%s:%s] Running...\n", test->ssuite->name, test->name);
    err = test->fn();
    if (err == 0) {
        printf("\tSuccess\n");
    } else {
        printf("\tFailed with error: %d\n", err);
    }

    return 0;
}

static int run_all_test_by_name(char *test_name) {
    const struct stest *stest_region = &start_stest_region;
    size_t stest_region_size = &end_stest_region - &start_stest_region;

    for (size_t i = 0; i < stest_region_size; i++) {
        if (stest_region[i].name != test_name) {
            continue;
        }

        int err = setjmp(buf);

        if (err != 0) {
            printf("\tFailed with signal %d\n", err);
            continue;
        }

        printf("[%s:%s] Running...\n", stest_region[i].ssuite->name, stest_region[i].name);
        err = stest_region[i].fn();
        if (err == 0) {
            printf("\tSuccess\n");
        } else {
            printf("\tFailed with error: %d\n", err);
        }
    }

    return 0;
}

int stest_run(int argc, char *argv[]) {

    struct sigaction sig_action;
    char *suite_name = NULL;
    char *test_name = NULL;

    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags = 0;
    sig_action.sa_handler = signal_handler;
    if (sigaction(SIGSEGV, &sig_action, NULL) < 0) {
        perror("sigaction");
        return -EFAULT;
    }

    handle_arguments(argc, argv, &suite_name, &test_name);

    if (suite_name == NULL && test_name == NULL) {
        return run_all_tests();
    }

    if (suite_name != NULL && test_name == NULL) {
        return run_all_suite_tests(suite_name);
    }

    if (suite_name == NULL && test_name != NULL) {
        return run_all_test_by_name(test_name);
    }

    if (suite_name != NULL && test_name != NULL) {
        return run_single_test(suite_name, test_name);
    }

    return 0;
}
