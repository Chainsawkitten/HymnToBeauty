#include "TestSuite.hpp"

#include <iostream>

using namespace std;

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

                if (test.function(test.data)) {
                    cout << "Success\n";
                    results.success++;
                } else {
                    cout << "Fail\n";
                    results.failure++;
                }
            } else {
                results.notRun++;
            }
        }
    } else {
        results.notRun += tests.size();
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

void TestSuite::AddTest(const string& name, bool (*function)(void*), void* data) {
    Test test;
    test.name = name;
    test.function = function;
    test.data = data;

    tests.push_back(test);
}
