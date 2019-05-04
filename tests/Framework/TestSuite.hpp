#pragma once

#include <string>
#include <vector>

/// A suite of tests, testing one module.
/**
 * @todo Linked list of suites.
 */
class TestSuite {
  public:
    /// The results of a run.
    struct Results {
        /// The number of failed tests.
        unsigned int failure = 0;

        /// The number of successful tests.
        unsigned int success = 0;

        /// The number of tests that weren't run.
        unsigned int notRun = 0;
    };

    /// Create a new test suite.
    /**
     * @param name The name of the test suite.
     */
    explicit TestSuite(const std::string& name);

    /// Destructor.
    ~TestSuite() {};

    /// Run the tests contained in the suite.
    /**
     * @param suiteName Name of the suite to run.
     * @param testName Name of the test to run.
     * @param results Results structure to append results to.
     */
    void Run(const std::string& suiteName, const std::string& testName, Results& results);

    /// Display the test contained in the suite.
    void Print() const;

  protected:
    /// Add a test to the suite.
    /**
     * @param name Name of the test.
     * @param function The function containing the test code.
     * @param data Data for the function to process.
     */
    void AddTest(const std::string& name, bool (*function)(void*), void* data = nullptr);

  private:
    std::string name;

    struct Test {
        std::string name;
        bool (*function)(void*);
        void* data;
    };
    std::vector<Test> tests;
};
