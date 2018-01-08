#include <string>

std::string DefaultScriptBody(R"(class Default {
    Hub @hub;
    Entity @self;

    Default(Entity @entity){
        @hub = Managers();
        @self = @entity;

        // Remove this if updates are not desired.
        RegisterUpdate();
    }

    // Called by the engine for each frame.
    void Update(float deltaTime) {

    }
}
)");
