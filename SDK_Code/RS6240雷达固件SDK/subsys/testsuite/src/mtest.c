/**
 ******************************************************************************
 * @file    mtest.c
 * @brief   micro test define.
 * @verbatim    null
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


/* Includes.
 * ----------------------------------------------------------------------------
 */
#include "hal_types.h"
#include "hal_dev.h"
#include "hal_board.h"
#include "mtest_test.h"


#if (CONFIG_MTEST == 1)
/* Private typedef.
 * ----------------------------------------------------------------------------
 */
/**
 * @brief Each enum member represents a distinct phase of execution for the
 *        test binary. TEST_PHASE_FRAMEWORK is active when internal ztest code
 *        is executing; the rest refer to corresponding phases of user test
 *        code.
 */
enum ztest_phase {
    TEST_PHASE_SETUP,
    TEST_PHASE_BEFORE,
    TEST_PHASE_TEST,
    TEST_PHASE_AFTER,
    TEST_PHASE_TEARDOWN,
    TEST_PHASE_FRAMEWORK
};

/**
 * @brief The current status of the test binary
 */
enum ztest_status {
    ZTEST_STATUS_OK,
    ZTEST_STATUS_HAS_FAILURE,
    ZTEST_STATUS_CRITICAL_ERROR
};

enum ztest_result {
    ZTEST_RESULT_PENDING,
    ZTEST_RESULT_PASS,
    ZTEST_RESULT_FAIL,
    ZTEST_RESULT_SKIP,
    ZTEST_RESULT_SUITE_SKIP,
    ZTEST_RESULT_SUITE_FAIL,
};


/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define NUM_ITER_PER_SUITE 1
#define NUM_ITER_PER_TEST  1

/* Private macros.
 * ----------------------------------------------------------------------------
 */



/* Private variables.
 * ----------------------------------------------------------------------------
 */
static bool failed_expectation;
static bool current_test_failed_assumption;
static enum ztest_phase phase = TEST_PHASE_FRAMEWORK;
static enum ztest_status test_status = ZTEST_STATUS_OK;
static enum ztest_result test_result;
static uint32_t tc_start_time;
static uint32_t tc_spend_time;

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

static inline const char *_ztest_result_to_str(int result);
static inline void _ztest_get_start_time_cyc(void);
static inline void _ztest_get_test_duration_ms(void);
static void _ztest_run_rules(bool is_before, struct ztest_unit_test *test, void *data);
static inline const char *_ztest_get_friendly_phase_name(enum ztest_phase phase);
static int _ztest_run(struct ztest_suite_node *suite, struct ztest_unit_test *test, void *data);
static struct ztest_suite_node *_ztest_find_test_suite(const char *name);
static struct ztest_unit_test *_ztest_get_next_test(const char *suite, struct ztest_unit_test *prev);
static void _ztest_show_suite_summary(struct ztest_suite_node *suite);
static void _ztest_show_suite_summary_verbose(struct ztest_suite_node *suite);
static void _test_show_suite_summary(enum ztest_status status);
static int _ztest_run_test_suite_ptr(struct ztest_suite_node *suite);
static void _ztest_init_unit_test_result_for_suite(struct ztest_suite_node *suite);
static bool _ztest_should_suite_run(const void *state, struct ztest_suite_node *suite);
static int _ztest_run_test_suite(struct ztest_suite_node *ptr, const void *state);


/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int ztest_run_test_suites(const void *state)
{
    int count = 0;
    enum ztest_status *pstate = (enum ztest_status *)state;

    if (*pstate == ZTEST_STATUS_CRITICAL_ERROR) {
        return count;
    }

    for (struct ztest_suite_node *ptr = _mtest_suite_node_list_start;
        ptr < _mtest_suite_node_list_end; ++ptr) {
        count += _ztest_run_test_suite(ptr, state);

        /* Stop running tests if we have a critical error */
        if (*pstate == ZTEST_STATUS_CRITICAL_ERROR) {
            break;
        }
    }

    return count;
}

void ztest_verify_all_test_suites_ran(void)
{
    bool all_tests_run = true;
    struct ztest_suite_node *suite;
    struct ztest_unit_test *test;

    for (suite = _mtest_suite_node_list_start; suite < _mtest_suite_node_list_end;
         ++suite) {
        if (suite->stats->run_count < 1) {
            TC_PRINT("ERROR: Test suite '%s' did not run.\n", suite->name);
            all_tests_run = false;
        }
    }

    for (test = _mtest_unit_test_list_start; test < _mtest_unit_test_list_end; ++test) {
        suite = _ztest_find_test_suite(test->test_suite_name);
        if (suite == NULL) {
            TC_PRINT("ERROR: Test '%s' assigned to test suite '%s' which doesn't exist\n",
                  test->name, test->test_suite_name);
            all_tests_run = false;
        }
    }

    if (!all_tests_run) {
        test_status = ZTEST_STATUS_HAS_FAILURE;
    }

    for (test = _mtest_unit_test_list_start; test < _mtest_unit_test_list_end; ++test) {
        if (test->stats->fail_count + test->stats->pass_count + test->stats->skip_count !=
            test->stats->run_count) {
            TC_PRINT("Bad stats for %s.%s\n", test->test_suite_name, test->name);
            test_status = ZTEST_STATUS_HAS_FAILURE;
        }
    }

}


void ztest_main(char *suit_name)
{
    ztest_run_test_suites(&test_status);
    ztest_verify_all_test_suites_ran();
    _test_show_suite_summary(test_status);
}

int z_ztest_run_test_suite(const char *name)
{
    return _ztest_run_test_suite_ptr(_ztest_find_test_suite(name));
}


static inline const char *_ztest_result_to_str(int result)
{
    switch (result) {
    case TC_PASS:
        return "PASS";
    case TC_FAIL:
        return "FAIL";
    case TC_SKIP:
        return "SKIP";
    default:
        return "?";
    }
}



static inline void _ztest_get_start_time_cyc(void)
{
    tc_start_time = HAL_BOARD_GetTime(HAL_TIME_MS);
}

static inline void _ztest_get_test_duration_ms(void)
{
    tc_spend_time = HAL_BOARD_GetTime(HAL_TIME_MS) - tc_start_time;
}


static void _ztest_run_rules(bool is_before, struct ztest_unit_test *test, void *data)
{
    for (struct ztest_test_rule *rule = _mtest_test_rule_list_start;
         rule < _mtest_test_rule_list_end; ++rule) {
        if (is_before && rule->before_each) {
            rule->before_each(test, data);
        } else if (!is_before && rule->after_each) {
            rule->after_each(test, data);
        }
    }
}

static inline const char *_ztest_get_friendly_phase_name(enum ztest_phase phase)
{
    switch (phase) {
    case TEST_PHASE_SETUP:
        return "setup";
    case TEST_PHASE_BEFORE:
        return "before";
    case TEST_PHASE_TEST:
        return "test";
    case TEST_PHASE_AFTER:
        return "after";
    case TEST_PHASE_TEARDOWN:
        return "teardown";
    case TEST_PHASE_FRAMEWORK:
        return "framework";
    default:
        return "(unknown)";
    }
}

void ztest_skip_failed_assumption(void)
{
    current_test_failed_assumption = true;
    ztest_test_skip();
}

void ztest_test_fail(void)
{
    switch (phase) {
    case TEST_PHASE_SETUP:
        test_result = ZTEST_RESULT_SUITE_FAIL;
        break;
    case TEST_PHASE_BEFORE:
    case TEST_PHASE_TEST:
        test_result = ZTEST_RESULT_FAIL;
        break;
    default:
        TC_PRINT(" ERROR: cannot fail in test phase '%s()', bailing\n",
              _ztest_get_friendly_phase_name(phase));
        test_status = ZTEST_STATUS_CRITICAL_ERROR;
        break;
    }
}

void ztest_test_pass(void)
{
    switch (phase) {
    case TEST_PHASE_TEST:
        test_result = ZTEST_RESULT_PASS;
        break;
    default:
        TC_PRINT(" ERROR: cannot pass in test phase '%s()', bailing\n",
              _ztest_get_friendly_phase_name(phase));
        test_status = ZTEST_STATUS_CRITICAL_ERROR;
    }
}

void ztest_test_skip(void)
{
    switch (phase) {
    case TEST_PHASE_SETUP:
        test_result = ZTEST_RESULT_SUITE_SKIP;
        break;
    case TEST_PHASE_BEFORE:
    case TEST_PHASE_TEST:
        test_result = ZTEST_RESULT_SKIP;
        break;
    default:
        TC_PRINT(" ERROR: cannot skip in test phase '%s()', bailing\n",
              _ztest_get_friendly_phase_name(phase));
        test_status = ZTEST_STATUS_CRITICAL_ERROR;
        break;
    }
}

void ztest_test_expect_fail(void)
{
    failed_expectation = true;
}


static int _ztest_run(struct ztest_suite_node *suite, struct ztest_unit_test *test, void *data)
{
    int ret = TC_PASS;

#if CONFIG_ZTEST_TEST_DELAY_MS > 0
    k_busy_wait(CONFIG_ZTEST_TEST_DELAY_MS * USEC_PER_MSEC);
#endif


    TC_PRINT("-----------------------\n");
    TC_PRINT(" - START - %s\n", test->name);

    phase = TEST_PHASE_BEFORE;

    /* If the suite's setup function marked us as skipped, don't bother
     * running the tests.
     */ 
    if (test_result != ZTEST_RESULT_SUITE_SKIP &&
           test_result != ZTEST_RESULT_SUITE_FAIL) {
        test_result = ZTEST_RESULT_PENDING;
        _ztest_get_start_time_cyc();
        _ztest_run_rules(/*is_before=*/true, test, data);
        if (suite->before) {
            suite->before(data);
        }

        phase = TEST_PHASE_TEST;
        test->test(data);
    }

    phase = TEST_PHASE_AFTER;
    if (suite->after != NULL) {
        suite->after(data);
    }
    _ztest_run_rules(/*is_before=*/false, test, data);

    _ztest_get_test_duration_ms();
    if (tc_spend_time > test->stats->duration_worst_ms) {
        test->stats->duration_worst_ms = tc_spend_time;
    }

    phase = TEST_PHASE_FRAMEWORK;

#if 0
    /* Flush all logs in case deferred mode and default logging thread are used. */
    while (IS_ENABLED(CONFIG_TEST_LOGGING_FLUSH_AFTER_TEST) &&
           IS_ENABLED(CONFIG_LOG_PROCESS_THREAD) && log_data_pending()) {
        k_msleep(100);
    }
#endif

    if (test_result == ZTEST_RESULT_FAIL || test_result == ZTEST_RESULT_SUITE_FAIL ||
        failed_expectation) {
        ret = TC_FAIL;
        failed_expectation = false;
    } else if (test_result == ZTEST_RESULT_SKIP || test_result == ZTEST_RESULT_SUITE_SKIP) {
        ret = TC_SKIP;
    }

    TC_PRINT(" - END - %s : %s in %u.%03u seconds\n",
        test->name,
        _ztest_result_to_str(ret),
        tc_spend_time/1000, tc_spend_time%1000);

    if (ret == TC_SKIP && current_test_failed_assumption) {
        test_status = 1;
    }

    return ret;
}


static struct ztest_suite_node *_ztest_find_test_suite(const char *name)
{
    struct ztest_suite_node *node;

    for (node = _mtest_suite_node_list_start; node < _mtest_suite_node_list_end; ++node) {
        if (strcmp(name, node->name) == 0) {
            return node;
        }
    }

    return NULL;
}

static struct ztest_unit_test *_ztest_get_next_test(const char *suite, struct ztest_unit_test *prev)
{
    struct ztest_unit_test *test = (prev == NULL) ? _mtest_unit_test_list_start : prev + 1;

    for (; test < _mtest_unit_test_list_end; ++test) {
        if (strcmp(suite, test->test_suite_name) == 0) {
            return test;
        }
    }

    return NULL;
}

static void _ztest_show_suite_summary(struct ztest_suite_node *suite)
{
    int distinct_pass = 0, distinct_fail = 0, distinct_skip = 0, distinct_total = 0;
    int effective_total = 0;
    int expanded_pass = 0, expanded_passrate = 0;
    int passrate_major = 0, passrate_minor = 0, passrate_tail = 0;
    int suite_result = TC_PASS;

    struct ztest_unit_test *test = NULL;
    unsigned int suite_duration_worst_ms = 0;

    /** summary of disctinct run  */
    while (((test = _ztest_get_next_test(suite->name, test)) != NULL)) {
        distinct_total++;
        suite_duration_worst_ms += test->stats->duration_worst_ms;
        if (test->stats->skip_count == test->stats->run_count) {
            distinct_skip++;
        } else if (test->stats->pass_count == test->stats->run_count) {
            distinct_pass++;
        } else {
            distinct_fail++;
        }
    }

    if (distinct_skip == distinct_total) {
        suite_result = TC_SKIP;
        passrate_major = passrate_minor = 0;
    } else {
        suite_result = (distinct_fail > 0) ? TC_FAIL : TC_PASS;
        effective_total = distinct_total - distinct_skip;
        expanded_pass = distinct_pass * 100000;
        expanded_passrate = expanded_pass / effective_total;
        passrate_major = expanded_passrate / 1000;
        passrate_minor = (expanded_passrate - passrate_major * 1000) / 10;
        passrate_tail = expanded_passrate - passrate_major * 1000 - passrate_minor * 10;
        if (passrate_tail >= 5) { /* rounding */
            passrate_minor++;
        }
    }

    TC_PRINT("SUITE [%s] : %s\n",
                suite->name,
                _ztest_result_to_str(suite_result));
    TC_PRINT("    - (%3d.%02d%%) total = %d, pass = %d, fail = %d, skip = %d\n",
                passrate_major, passrate_minor,
                distinct_total,
                distinct_pass, distinct_fail, distinct_skip);
    TC_PRINT("    - duration = %u.%03u seconds\n",
                suite_duration_worst_ms / 1000,
                suite_duration_worst_ms % 1000);
}

static void _ztest_show_suite_summary_verbose(struct ztest_suite_node *suite)
{
    struct ztest_unit_test *test = NULL;
    int tc_result = TC_PASS;

    while (((test = _ztest_get_next_test(suite->name, test)) != NULL)) {
        if (test->stats->skip_count == test->stats->run_count) {
            tc_result = TC_SKIP;
        } else if (test->stats->pass_count == test->stats->run_count) {
            tc_result = TC_PASS;
        } else {
            tc_result = TC_FAIL;
        }

        TC_PRINT(" - %s - [%s] duration = %u.%03u seconds\n",
                _ztest_result_to_str(tc_result),
                test->name,
                test->stats->duration_worst_ms / 1000,
                test->stats->duration_worst_ms % 1000);
    }

    TC_PRINT("\n");

}

static void _test_show_suite_summary(enum ztest_status status)
{
    TC_PRINT("============================================================\n");
    TC_PRINT("PROJECT EXECUTION %s\n", (status) == TC_PASS ? "SUCCESSFUL" : "FAILED");

    /* Flush the log a lot to ensure that no summary content
     * is dropped if it goes through the logging subsystem.
     */
    TC_PRINT("\n------ TESTSUITE SUMMARY START ------\n\n");

    for (struct ztest_suite_node *ptr = _mtest_suite_node_list_start;
         ptr < _mtest_suite_node_list_end; ++ptr) {

        _ztest_show_suite_summary(ptr);
        _ztest_show_suite_summary_verbose(ptr);
    }

    TC_PRINT("------ TESTSUITE SUMMARY END --------\n\n");
}


static int _ztest_run_test_suite_ptr(struct ztest_suite_node *suite)
{
    struct ztest_unit_test *test = NULL;
    void *data = NULL;
    int fail = 0;
    int tc_result = TC_PASS;

    if (test_status < 0) {
        return test_status;
    }

    if (suite == NULL) {
        test_status = ZTEST_STATUS_CRITICAL_ERROR;
        return -1;
    }

    TC_PRINT("START TESTSUITE [%s]\n", suite->name);
    TC_PRINT("------------------------------------------------------------\n");

    current_test_failed_assumption = false;
    test_result = ZTEST_RESULT_PENDING;
    phase = TEST_PHASE_SETUP;

    if (test_result != ZTEST_RESULT_SUITE_FAIL && suite->setup != NULL) {
        data = suite->setup();
    }

    fail = 0;
    while (((test = _ztest_get_next_test(suite->name, test)) != NULL)) {
        test->stats->run_count++;
        tc_result = _ztest_run(suite, test, data);
        if (tc_result == TC_PASS) {
            test->stats->pass_count++;
        } else if (tc_result == TC_SKIP) {
            test->stats->skip_count++;
        } else if (tc_result == TC_FAIL) {
            test->stats->fail_count++;
        }

        if (tc_result == TC_FAIL) {
            fail++;
        }

        if (test_status == ZTEST_STATUS_CRITICAL_ERROR) {
            break;
        }
    }

    if (test_status == ZTEST_STATUS_OK && fail != 0) {
        test_status = ZTEST_STATUS_HAS_FAILURE;
    }

    TC_PRINT("------------------------------------------------------------\n");
    TC_PRINT(" END TESTSUITE [%s]\n", suite->name);

    phase = TEST_PHASE_TEARDOWN;
    if (suite->teardown != NULL) {
        suite->teardown(data);
    }

    return fail;
}

static void _ztest_init_unit_test_result_for_suite(struct ztest_suite_node *suite)
{
    struct ztest_unit_test *test = NULL;

    while (((test = _ztest_get_next_test(suite->name, test)) != NULL)) {
        test->stats->run_count = 0;
        test->stats->skip_count = 0;
        test->stats->fail_count = 0;
        test->stats->pass_count = 0;
        test->stats->duration_worst_ms = 0;
    }
}

static bool _ztest_should_suite_run(const void *state, struct ztest_suite_node *suite)
{
    bool run_suite = true;

    if (suite->predicate != NULL) {
        run_suite = suite->predicate(state);
    }

    return run_suite;
}

static int _ztest_run_test_suite(struct ztest_suite_node *ptr, const void *state)
{
    struct ztest_suite_stats *stats = ptr->stats;
    int count = 0;

    if (_ztest_should_suite_run(state, ptr)) {
        _ztest_init_unit_test_result_for_suite(ptr);
        int fail = _ztest_run_test_suite_ptr(ptr);

        count++;
        stats->run_count++;
        stats->fail_count += (fail != 0) ? 1 : 0;
    } else {
        stats->skip_count++;
    }

    return count;
}

#endif /* CONFIG_MTEST */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
