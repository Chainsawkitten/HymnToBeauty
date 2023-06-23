#include "TestSuite.hpp"

#include <iostream>

using namespace std;

Result ToResult(bool success) {
    return success ? Result::SUCCESS : Result::FAILURE;
}

TestSuite::TestSuite(const string& name) {
    this->name = name;
}

void TestSuite::Run(const string& suiteName, const string& testName, Results& results) {
    if (suiteName.empty() || name == suiteName) {
        cout << "=========================\n";
        cout << name << std::endl;
        cout << "=========================\n";

        for (Test& test : tests) {
            if (testName.empty() || test.name == testName) {
                cout << test.name << ": ";

                Result result = test.function(test.data);
                switch (result) {
                case Result::SUCCESS:
                    cout << "Success\n";
                    results.success++;
                    break;
                case Result::FAILURE:
                    cout << "Fail\n";
                    results.failure++;
                    break;
                case Result::UNSUPPORTED:
                    cout << "Unsupported\n";
                    results.unsupported++;
                    break;
                }
            } else {
                results.skipped++;
            }
        }
    } else {
        results.skipped += tests.size();
    }
}

void TestSuite::Print() const {
    cout << "=========================\n";
    cout << name << std::endl;
    cout << "=========================\n";

    for (const Test& test : tests) {
        cout << test.name << endl;
    }
}

void TestSuite::AddTest(const string& name, Result (*function)(void*), void* data) {
    Test test;
    test.name = name;
    test.function = function;
    test.data = data;

    tests.push_back(test);
}
