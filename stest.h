/**
 * @file stest.h
 * @author Matheus T. dos Santos (matheuswhite1@protonmail.com)
 * @brief
 * @version 0.1
 * @date 01/11/24
 *
 * @copyright Copyright (c) 2024 Citrinio All rights reserved.
 */
#ifndef STEST_H
#define STEST_H

#define SSUITE_REGION_SECTION __attribute__((used, section("__DATA,__ssuite_region")))
#define STEST_REGION_SECTION __attribute__((used, section("__DATA,__stest_region")))
#define SSUITE(name_) static struct ssuite SSUITE_REGION_SECTION name_ = {.name = #name_}
#define STEST(suite, test, ...)                                                                                        \
    static int test##_fn(__VA_ARGS__);                                                                                 \
    __attribute__((unused)) static struct stest STEST_REGION_SECTION test = {                                          \
            .ssuite = &suite, .name = #test, .fn = test##_fn};                                                         \
    static int test##_fn(__VA_ARGS__)

typedef int (*stest_test_fn_t)();

struct ssuite {
    char *name;
};

struct stest {
    struct ssuite *ssuite;
    char *name;
    stest_test_fn_t fn;
};

int stest_run(int argc, char **argv);

#endif /* STEST_H */
