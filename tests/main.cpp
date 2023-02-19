#include <iostream>
#include <string>
#include <cstring>

#include "Video/VideoSuite.hpp"

using namespace std;

// Print usage instructions.
static void PrintHelp() {
    /// @todo Command line option to write PNG (even if there was no failure)?
    cout << "Parameters:\n"
            "  --suite name  Only run the suite with the given name.\n"
            "  --test name   Only run the test with the given name.\n"
            "  --list        Display a list of all suites and the tests in them.\n"
            "  --help        Display this help.\n"
            "  --vulkan      Use Vulkan.\n"
            "  --webgpu      Use WebGPU.\n";
}

int main(int argc, char* argv[]) {
    // Process arguments.
    string suite = "";
    string test = "";
    bool help = false;
    bool list = false;
    Video::Renderer::GraphicsAPI graphicsAPI = Video::Renderer::GraphicsAPI::OPENGL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--suite") == 0 && argc > i + 1) {
            suite = argv[++i];
        } else if (strcmp(argv[i], "--test") == 0 && argc > i + 1) {
            test = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0) {
            help = true;
        } else if (strcmp(argv[i], "--list") == 0) {
            list = true;
        } else if (strcmp(argv[i], "--vulkan") == 0) {
            graphicsAPI = Video::Renderer::GraphicsAPI::VULKAN;
        } else if (strcmp(argv[i], "--webgpu") == 0) {
            graphicsAPI = Video::Renderer::GraphicsAPI::WEBGPU;
        }
    }

    if (help) {
        // Display help.
        PrintHelp();
    } else {
        // Create suites.
        VideoSuite videoSuite(graphicsAPI);

        if (list) {
            // Display list of all suites and tests.
            videoSuite.Print();
        } else {
            TestSuite::Results results;

            // Run all tests matching the suite and test names.
            videoSuite.Init();
            videoSuite.Run(suite, test, results);
            videoSuite.Shutdown();

            // Print summary of results.
            const unsigned int total = results.success + results.failure + results.notRun;
            cout << "\n\n"
                 << "Results:\n"
                 << "   Success: " << results.success << " / " << total << "\n"
                 << "   Failure: " << results.failure << " / " << total << "\n"
                 << "   Not run: " << results.notRun << " / " << total << "\n";

            if (results.failure > 0) {
                return 1;
            }
        }
    }

    return 0;
}
