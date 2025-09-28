#ifndef VIEWPORT
#define VIEWPORT

#include "common.cpp"
#include "render.cpp"
#include "shader.cpp"
#include "framebuffer.cpp"


struct Viewport{
    #define QUAD_ARRAY_SIZE 30

    Framebuffer m_framebuffer;
    GLRenderObject m_renderable;
    Shader m_shader;
    
    glm::ivec2 m_position;
    glm::ivec2 m_size;

    void Initialize(glm::ivec2 size, glm::ivec2 position){
        m_size = size;
        m_position = position;

        glGenVertexArrays(1, &m_renderable.vao);
        glGenBuffers(1, &m_renderable.vbo);
        GLBufferData(&m_renderable, VERTEX_BUFFER, GL_DYNAMIC_DRAW, nullptr, QUAD_ARRAY_SIZE*sizeof(float), sizeof(float));
        GLAddAttribute(&m_renderable, GL_FLOAT, 3, 5*sizeof(float), 0);
        GLAddAttribute(&m_renderable, GL_FLOAT, 2, 5*sizeof(float), 3*sizeof(float));

        CreateQuad();

        m_framebuffer.Initialize(m_size.x, m_size.y);

        m_shader = {};
        m_shader.Create("framebuffer_vertex.glsl", "framebuffer_fragment.glsl");
    }

    void BindFramebuffer(){
        GLFramebufferBind(m_framebuffer.m_ID);
    }

    void UnbindFramebuffer(){
        GLFramebufferBind(0);
    }

    void Update(glm::ivec2 size, glm::ivec2 position){
        m_size = size;
        m_position = position;

        CreateQuad();
        m_framebuffer.Resize(m_size.x, m_size.y);
    }

    void Draw(){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);  // Make sure it draws over everything
        
        m_shader.Use();
        TextureUse(m_framebuffer.m_textureID, GL_TEXTURE0);
        
        glm::mat4 ortho = glm::ortho(0.0f, (float)m_size.x, 0.0f, (float)m_size.y);
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(ortho));

        GLDraw(&m_renderable, DRAW_ARRAY, GL_TRIANGLES, 6);

        glEnable(GL_DEPTH_TEST);  // Restore for next frame
        glDisable(GL_BLEND);
    }

private:
    void CreateQuad(){
        float x = m_position.x;
        float y = m_position.y;
        float w = m_size.x;
        float h = m_size.y;

        // 6 vertices × 5 floats each = 30 floats per quad
        float quadVertices[QUAD_ARRAY_SIZE] = {
            // positions       // texcoords
            x,     y,     0.0f, 0.0f, 0.0f, // bottom-left
            x + w, y + h, 0.0f, 1.0f, 1.0f, // top-right
            x + w, y,     0.0f, 1.0f, 0.0f, // bottom-right

            x,     y,     0.0f, 0.0f, 0.0f, // bottom-left
            x,     y + h, 0.0f, 0.0f, 1.0f, // top-left
            x + w, y + h, 0.0f, 1.0f, 1.0f  // top-right
        };

        glBindVertexArray(m_renderable.vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_renderable.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, QUAD_ARRAY_SIZE*sizeof(float), quadVertices);
    }
};




#endif