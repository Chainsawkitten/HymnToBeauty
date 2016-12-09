#pragma once

#include <GLFW\glfw3.h>
#include <Engine\MainWindow.hpp>
#include <vector>


/// Interface for renderable 2D geometry.
class Input {

public:

    static GLFWwindow* window;

    static void AddButton(int key, int action, std::string desc = "");
    static bool Check_Button(int index);

private:

    static class Button {

    public:

        Button(int key, int action, std::string desc) {

            this->key = key;
            this->action = action;
            this->desc = desc;

        }

        std::string desc;
        int key;
        int action;
        
    };
    
    static std::vector<Button> buttons;
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

};
