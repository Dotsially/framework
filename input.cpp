#ifndef INPUT
#define INPUT

#include "common.cpp"
#include "window.cpp"

struct MouseButton{
    bool leftIsPressed = false;
    bool rightIsPressed = false; 
    bool middleIsPressed = false;
};

struct InputState{
    glm::vec2 mousePosition;
    glm::vec2 mouseDelta;
    glm::vec2 inputVector;
    MouseButton mouseButtonStates;
    float mouseScroll;
    const bool* keystate;
};


void InputHandler(InputState& state, Window& window) {
    state.inputVector = glm::vec2(0);
    state.mouseScroll = 0;  // Reset scroll delta every frame

    // Process all pending events
    while (SDL_PollEvent(&window.event)) {
        switch (window.event.type) {
            case SDL_EVENT_QUIT:
                window.quit = true;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                WindowResize(window);
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                state.mouseScroll += window.event.wheel.y;  // scroll up: +1, scroll down: -1
                break;
        }
    }

    SDL_PumpEvents();
    state.keystate = SDL_GetKeyboardState(NULL);

    uint32_t mouseButton = SDL_GetMouseState(&state.mousePosition.x, &state.mousePosition.y);
    SDL_GetRelativeMouseState(&state.mouseDelta.x, &state.mouseDelta.y);

    state.mouseButtonStates.leftIsPressed  = mouseButton & SDL_BUTTON_LMASK;
    state.mouseButtonStates.rightIsPressed = mouseButton & SDL_BUTTON_RMASK;
    state.mouseButtonStates.middleIsPressed = mouseButton & SDL_BUTTON_MIDDLE;

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