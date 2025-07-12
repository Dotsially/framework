#ifndef WINDOW
#define WINDOW

#include "common.cpp"

struct Window{
    std::string title;
    SDL_GLContext glContext;
    SDL_Event event;
    SDL_Window* handle;
    int32_t width;
    int32_t height;
    int32_t minWidth = 640;
    int32_t minHeight = 360;
    bool quit = false;
};

int WindowInit(Window* window, int32_t width, int32_t height, std::string title){
    window->width = 1280;
    window->height = 720;
    window->title = title;

    if (!SDL_Init(SDL_INIT_VIDEO)){
        std::cout << "Failed to init SDL\n"; 
        return -1;
    }
    
    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);  
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    window->handle = SDL_CreateWindow(window->title.c_str(), window->width, window->height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window->handle){
        std::cout << "Failed to create a window \n";
        return -1;
    }

    window->glContext = SDL_GL_CreateContext(window->handle);
    if (!window->glContext){
        std::cout << "Failed to create a gl context \n";
        return -1;
    }

    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
        std::cout << "Failed to load glad";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    //Face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    return 0;
}

void WindowResize(Window& window){
    SDL_GetWindowSize(window.handle, &window.width, &window.height);
    if (window.width < window.minWidth) window.width = window.minWidth;
    if (window.height < window.minHeight) window.height = window.minHeight;
    SDL_SetWindowSize(window.handle, window.width, window.height);
    glViewport(0, 0, window.width, window.height);
}

void WindowSwapBuffer(Window& window){
    SDL_GL_SwapWindow(window.handle);
}

#endif