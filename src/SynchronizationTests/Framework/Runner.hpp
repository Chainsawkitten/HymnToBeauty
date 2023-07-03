#pragma once

#include <Video/Renderer.hpp>

class SynchronizationTest;

/// Run a synchronization test.
/**
 * @param test The test to run.
 * @param window Window to present to.
 * @param graphicsAPI Which graphics API to use.
 */
void RunTest(SynchronizationTest* test, Utility::Window* window, Video::Renderer::GraphicsAPI graphicsAPI);

/// Setup the test.
/**
 * @param test The test to run.
 * @param window Window to present to.
 * @param graphicsAPI Which graphics API to use.
 */
void SetupTest(SynchronizationTest* test, Utility::Window* window, Video::Renderer::GraphicsAPI graphicsAPI);

/// Run a frame of the test.
void RunFrame();

/// Clean up test.
void ShutdownTest();
