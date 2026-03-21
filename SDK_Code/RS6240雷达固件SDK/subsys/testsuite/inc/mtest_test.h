/**
 ******************************************************************************
 * @file    mtest_test.h
 * @brief   min test define.
 ******************************************************************************
 * @attention
 *
 * Copyright (C) 2025 POSSUMIC TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of POSSUMIC TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#ifndef _MTEST_TEST_H
#define _MTEST_TEST_H

/* Includes.
 * ----------------------------------------------------------------------------
 */


#include "hal_types.h"
#include "mtest_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (CONFIG_MTEST == 1)
/* Exported types.
 * ----------------------------------------------------------------------------
 */

/* Exported constants.
 * ----------------------------------------------------------------------------
 */

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

#ifndef TC_PASS
#define TC_PASS 0
#endif

#ifndef TC_FAIL
#define TC_FAIL 1
#endif

#ifndef TC_SKIP
#define TC_SKIP 2
#endif

#ifndef TC_PRINT
#define TC_PRINT(fmt, ...)      printf(fmt, ##__VA_ARGS__)
#endif

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

struct ztest_unit_test {
    const char *test_suite_name;
    const char *name;
    void (*test)(void *data);

    /** Stats */
    struct ztest_unit_test_stats *const stats;
};

extern struct ztest_unit_test _mtest_unit_test_list_start[];
extern struct ztest_unit_test _mtest_unit_test_list_end[];
#define ZTEST_TEST_COUNT (_mtest_unit_test_list_end - _mtest_unit_test_list_start)

/**
 * Stats about a ztest suite
 */
struct ztest_suite_stats {
    /** The number of times that the suite ran */
    uint32_t run_count;
    /** The number of times that the suite was skipped */
    uint32_t skip_count;
    /** The number of times that the suite failed */
    uint32_t fail_count;
};

struct ztest_unit_test_stats {
    /** The number of times that the test ran */
    uint32_t run_count;
    /** The number of times that the test was skipped */
    uint32_t skip_count;
    /** The number of times that the test failed */
    uint32_t fail_count;
    /** The number of times that the test passed */
    uint32_t pass_count;
    /** The longest duration of the test across multiple times */
    uint32_t duration_worst_ms;
};

/**
 * Setup function to run before running this suite
 *
 * @return Pointer to the data structure that will be used throughout this test suite
 */
typedef void *(*ztest_suite_setup_t)(void);

/**
 * Function to run before each test in this suite
 *
 * @param fixture The test suite's fixture returned from setup()
 */
typedef void (*ztest_suite_before_t)(void *fixture);

/**
 * Function to run after each test in this suite
 *
 * @param fixture The test suite's fixture returned from setup()
 */
typedef void (*ztest_suite_after_t)(void *fixture);

/**
 * Teardown function to run after running this suite
 *
 * @param fixture The test suite's data returned from setup()
 */
typedef void (*ztest_suite_teardown_t)(void *fixture);

/**
 * An optional predicate function to determine if the test should run. If NULL, then the
 * test will only run once on the first attempt.
 *
 * @param global_state The current state of the test application.
 * @return True if the suite should be run; false to skip.
 */
typedef bool (*ztest_suite_predicate_t)(const void *global_state);

/**
 * A single node of test suite. Each node should be added to a single linker section which will
 * allow ztest_run_test_suites() to iterate over the various nodes.
 */
struct ztest_suite_node {
    /** The name of the test suite. */
    const char *const name;

    /** Setup function */
    const ztest_suite_setup_t setup;

    /** Before function */
    const ztest_suite_before_t before;

    /** After function */
    const ztest_suite_after_t after;

    /** Teardown function */
    const ztest_suite_teardown_t teardown;

    /** Optional predicate filter */
    const ztest_suite_predicate_t predicate;

    /** Stats */
    struct ztest_suite_stats *const stats;
};

extern struct ztest_suite_node _mtest_suite_node_list_start[];
extern struct ztest_suite_node _mtest_suite_node_list_end[];
#define ZTEST_SUITE_COUNT (_mtest_suite_node_list_end - _mtest_suite_node_list_start)

/**
 * Create and register a ztest suite. Using this macro creates a new test suite (using
 * ztest_test_suite). It then creates a struct ztest_suite_node in a specific linker section.
 *
 * Tests can then be run by calling ztest_run_test_suites(const void *state) by passing
 * in the current state. See the documentation for ztest_run_test_suites for more info.
 *
 * @param SUITE_NAME The name of the suite (see ztest_test_suite for more info)
 * @param PREDICATE A function to test against the state and determine if the test should run.
 * @param setup_fn The setup function to call before running this test suite
 * @param before_fn The function to call before each unit test in this suite
 * @param after_fn The function to call after each unit test in this suite
 * @param teardown_fn The function to call after running all the tests in this suite
 */
#define ZTEST_SUITE(SUITE_NAME, PREDICATE, setup_fn, before_fn, after_fn, teardown_fn)             \
    struct ztest_suite_stats z_ztest_suite_node_stats_##SUITE_NAME;                  \
    const __attribute__((section("._iterables_mtest_suite_node.static"))) struct ztest_suite_node z_ztest_test_node_const_##SUITE_NAME = {         \
        .name = #SUITE_NAME,                                                     \
        .setup = (setup_fn),                                                               \
        .before = (before_fn),                                                             \
        .after = (after_fn),                                                               \
        .teardown = (teardown_fn),                                                         \
        .predicate = PREDICATE,                                                            \
        .stats = &z_ztest_suite_node_stats_##SUITE_NAME,             \
    }
/**
 * Default entry point for running or listing registered unit tests.
 *
 * @param state The current state of the machine as it relates to the test executable.
 */
void ztest_run_all(const void *state);

/**
 * Run the registered unit tests which return true from their predicate function.
 *
 * @param state The current state of the machine as it relates to the test executable.
 * @return The number of tests that ran.
 */

int ztest_run_test_suites(const void *state);



/**
 * @brief Fails the test if any of the registered tests did not run.
 *
 * When registering test suites, a pragma function can be provided to determine WHEN the test should
 * run. It is possible that a test suite could be registered but the pragma always prevents it from
 * running. In cases where a test should make sure that ALL suites ran at least once, this function
 * may be called at the end of test_main(). It will cause the test to fail if any suite was
 * registered but never ran.
 */
void ztest_verify_all_test_suites_ran(void);

/**
 * @brief Run a test suite.
 *
 * Internal implementation. Do not call directly. This will run the full test suite along with some
 * checks for fast failures and initialization.
 *
 * @param name The name of the suite to run.
 * @return Negative value if the test suite never ran; otherwise, return the number of failures.
 */
int z_ztest_run_test_suite(const char *name);

/**
 * @brief Returns next test within suite.
 *
 * @param suite Name of suite to get next test from.
 * @param prev  Previous unit test acquired from suite, use NULL to return first
 *        unit test.
 * @return struct ztest_unit_test*
 */
struct ztest_unit_test *z_ztest_get_next_test(const char *suite, struct ztest_unit_test *prev);



/**
 * @defgroup ztest_test Ztest testing macros
 * @ingroup ztest
 *
 * This module eases the testing process by providing helpful macros and other
 * testing structures.
 *
 * @{
 */

/**
 * @brief Fail the currently running test.
 *
 * This is the function called from failed assertions and the like. You
 * probably don't need to call it yourself.
 */
void ztest_test_fail(void);

/**
 * @brief Pass the currently running test.
 *
 * Normally a test passes just by returning without an assertion failure.
 * However, if the success case for your test involves a fatal fault,
 * you can call this function from k_sys_fatal_error_handler to indicate that
 * the test passed before aborting the thread.
 */
void ztest_test_pass(void);

/**
 * @brief Skip the current test.
 *
 */
void ztest_test_skip(void);


void ztest_skip_failed_assumption(void);

#define Z_TEST(suite, fn, t_options, use_fixture)                                                  \
    struct ztest_unit_test_stats z_ztest_unit_test_stats_##suite##_##fn;                       \
    static void _##suite##_##fn##_wrapper(void *data);                                         \
    static void suite##_##fn(void);              \
    __attribute__((section("._iterables_mtest_unit_test.static"))) struct ztest_unit_test z_ztest_unit_test__##suite##__##fn = {    \
        .test_suite_name = #suite,                                               \
        .name = #fn,                                                             \
        .test = (_##suite##_##fn##_wrapper),                                               \
        .stats = &z_ztest_unit_test_stats_##suite##_##fn                                   \
    };                                                                                         \
    static void _##suite##_##fn##_wrapper(void *data)                                          \
    {                                                                                          \
        suite##_##fn();                                   \
    }                                                                                          \
    static inline void suite##_##fn(void)

#define Z_ZTEST(suite, fn, t_options) Z_TEST(suite, fn, t_options, 0)

/**
 * @brief Skips the test if config is enabled
 *
 * Use this macro at the start of your test case, to skip it when
 * config is enabled.  Useful when your test is still under development.
 *
 * @param config The Kconfig option used to skip the test.
 */
// #define Z_TEST_SKIP_IFDEF(config) COND_CODE_1(config, (ztest_test_skip()), ())

/**
 * @brief Skips the test if config is not enabled
 *
 * Use this macro at the start of your test case, to skip it when
 * config is not enabled.  Useful when your need to skip test if some
 * conifiguration option is not enabled.
 *
 * @param config The Kconfig option used to skip the test (if not enabled).
 */
// #define Z_TEST_SKIP_IFNDEF(config) COND_CODE_1(config, (), (ztest_test_skip()))

/**
 * @brief Create and register a new unit test.
 *
 * Calling this macro will create a new unit test and attach it to the declared `suite`. The `suite`
 * does not need to be defined in the same compilation unit.
 *
 * @param suite The name of the test suite to attach this test
 * @param fn The test function to call.
 */
#define ZTEST(suite, fn) Z_ZTEST(suite, fn, 0)


/**
 * @brief Define a test function
 *
 * This macro behaves exactly the same as ZTEST(), but the function takes an argument for the
 * fixture of type `struct suite##_fixture*` named `this`.
 *
 * @param suite The name of the test suite to attach this test
 * @param fn The test function to call.
 */
#define ZTEST_F(suite, fn) Z_ZTEST_F(suite, fn, 0)


/**
 * @brief Test rule callback function signature
 *
 * The function signature that can be used to register a test rule's before/after callback. This
 * provides access to the test and the fixture data (if provided).
 *
 * @param test Pointer to the unit test in context
 * @param data Pointer to the test's fixture data (may be NULL)
 */
typedef void (*ztest_rule_cb)(const struct ztest_unit_test *test, void *data);

/** @private */
struct ztest_test_rule {
    ztest_rule_cb before_each;
    ztest_rule_cb after_each;
};

/**
 * @brief Define a test rule that will run before/after each unit test.
 *
 * Functions defined here will run before/after each unit test for every test suite. Along with the
 * callback, the test functions are provided a pointer to the test being run, and the data. This
 * provides a mechanism for tests to perform custom operations depending on the specific test or
 * the data (for example logging may use the test's name).
 *
 * Ordering:
 * - Test rule's `before` function will run before the suite's `before` function. This is done to
 * allow the test suite's customization to take precedence over the rule which is applied to all
 * suites.
 * - Test rule's `after` function is not guaranteed to run in any particular order.
 *
 * @param name The name for the test rule (must be unique within the compilation unit)
 * @param before_each_fn The callback function (ztest_rule_cb) to call before each test
 *        (may be NULL)
 * @param after_each_fn The callback function (ztest_rule_cb) to call after each test (may be NULL)
 */
#define ZTEST_RULE(name, before_each_fn, after_each_fn)                                            \
    static STRUCT_SECTION_ITERABLE(ztest_test_rule, z_mtest_test_rule_##name) = {              \
        .before_each = (before_each_fn),                                                   \
        .after_each = (after_each_fn),                                                     \
    }

extern struct ztest_test_rule _mtest_test_rule_list_start[];
extern struct ztest_test_rule _mtest_test_rule_list_end[];


/**
 * @brief Run the specified test suite.
 *
 * @param suite Test suite to run.
 */
#define ztest_run_test_suite(suite) z_ztest_run_test_suite(STRINGIFY(suite))



/**
 * @}
 */

#endif /* CONFIG_MTEST */

#ifdef __cplusplus
}
#endif

#endif /* _MTEST_TEST_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

