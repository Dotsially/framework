#ifndef INPUT
#define INPUT

#include "common.cpp"
#include "window.cpp"

struct MouseButton {
    bool leftIsPressed = false;
    bool rightIsPressed = false; 
    bool middleIsPressed = false;

    bool leftWasPressed = false;
    bool rightWasPressed = false;
    bool middleWasPressed = false;

    bool leftReleased = false;
    bool rightReleased = false;
    bool middleReleased = false;
};

struct InputState{
    glm::vec2 mousePosition;
    glm::vec2 mouseDelta;
    glm::vec2 inputVector;
    MouseButton mouseButtonStates;
    const bool* keystate;
    float mouseScroll;
    bool windowRezised = false;
};


void InputHandler(InputState& state, Window& window) {
    state.inputVector = glm::vec2(0);
    state.mouseScroll = 0;  // Reset scroll delta every frame
    state.windowRezised = false;

    // Process all pending events
    while (SDL_PollEvent(&window.m_event)) {
        switch (window.m_event.type) {
            case SDL_EVENT_QUIT:
                window.m_quit = true;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                window.Resize();
                state.windowRezised = true;
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                state.mouseScroll += window.m_event.wheel.y;  // scroll up: +1, scroll down: -1
                break;
        }
    }

    SDL_PumpEvents();
    state.keystate = SDL_GetKeyboardState(NULL);

    uint32_t mouseButton = SDL_GetMouseState(&state.mousePosition.x, &state.mousePosition.y);
    SDL_GetRelativeMouseState(&state.mouseDelta.x, &state.mouseDelta.y);

    // Save previous state
    state.mouseButtonStates.leftWasPressed = state.mouseButtonStates.leftIsPressed;
    state.mouseButtonStates.rightWasPressed = state.mouseButtonStates.rightIsPressed;
    state.mouseButtonStates.middleWasPressed = state.mouseButtonStates.middleIsPressed;

    state.mouseButtonStates.leftIsPressed   = (mouseButton & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0;
    state.mouseButtonStates.rightIsPressed  = (mouseButton & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) != 0;
    state.mouseButtonStates.middleIsPressed = (mouseButton & SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE)) != 0;

    // Detect release
    state.mouseButtonStates.leftReleased = state.mouseButtonStates.leftWasPressed && !state.mouseButtonStates.leftIsPressed;
    state.mouseButtonStates.rightReleased = state.mouseButtonStates.rightWasPressed && !state.mouseButtonStates.rightIsPressed;
    state.mouseButtonStates.middleReleased = state.mouseButtonStates.middleWasPressed && !state.mouseButtonStates.middleIsPressed;

    state.inputVector.x = 0;
    state.inputVector.y = 0;

    if (state.keystate[SDL_SCANCODE_W]) {
        state.inputVector.y -= 1;
    }
    if (state.keystate[SDL_SCANCODE_S]) {
        state.inputVector.y += 1;
    }
    if (state.keystate[SDL_SCANCODE_A]) {
        state.inputVector.x -= 1;
    }
    if (state.keystate[SDL_SCANCODE_D]) {
        state.inputVector.x += 1;
    }
}


#endif