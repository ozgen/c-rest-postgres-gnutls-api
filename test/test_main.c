//
// Created by Mehmet Ozgen on 11.01.2025.
//
#include <cgreen/cgreen.h>
#include <cgreen/runner.h>

// Declare test suites from your test files
TestSuite *DataRepository_tests();
TestSuite *Dotenv_tests();

int main(int argc, char **argv) {
    TestSuite *suite = create_test_suite();
    add_suite(suite, DataRepository_tests());
    add_suite(suite, Dotenv_tests());
    return run_test_suite(suite, create_text_reporter());
}
