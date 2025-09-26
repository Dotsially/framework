#ifndef WINDOW
#define WINDOW

#include "common.cpp"
#include "common_graphics.cpp"

class Window{
public:
    static Window& Instance() {
        static Window instance = {};         
        return instance;
    }

    std::string m_title;
    SDL_GLContext m_glContext;
    SDL_Event m_event;
    SDL_Window* m_handle;
    int32_t m_width;
    int32_t m_height;
    int32_t m_minWidth = 640;
    int32_t m_minHeight = 360;
    bool m_quit = false;

    int Initialize(int32_t width, int32_t height, std::string title){
        m_width = width;
        m_height = height;
        m_title = title;

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

        m_handle = SDL_CreateWindow(m_title.c_str(), m_width, m_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        if (!m_handle){
            std::cout << "Failed to create a window \n";
            return -1;
        }

        m_glContext = SDL_GL_CreateContext(m_handle);
        if (!m_glContext){
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

    void Resize(){
        SDL_GetWindowSize(m_handle, &m_width, &m_height);
        
        if (m_width < m_minWidth) 
            m_width = m_minWidth;

        if (m_height < m_minHeight) 
            m_height = m_minHeight;

        SDL_SetWindowSize(m_handle, m_width, m_height);
        glViewport(0, 0, m_width, m_height);
    }

    void SwapBuffer(){
        SDL_GL_SwapWindow(m_handle);
    }
};

#endif