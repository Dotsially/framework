#ifndef VIEWPORT
#define VIEWPORT

#include "common.cpp"
#include "render.cpp"
#include "shader.cpp"
#include "framebuffer.cpp"

#define QUAD_ARRAY_SIZE 30

struct Viewport{
    Framebuffer framebuffer;
    GLRenderObject renderable;
    Shader shader;
    
    glm::ivec2 position;
    glm::ivec2 size;
};

void ViewportCreateQuad(Viewport* viewport){
    float x = viewport->position.x;
    float y = viewport->position.y;
    float w = viewport->size.x;
    float h = viewport->size.y;

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

    glBindVertexArray(viewport->renderable.vao);
    glBindBuffer(GL_ARRAY_BUFFER, viewport->renderable.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, QUAD_ARRAY_SIZE*sizeof(float), quadVertices);
}

void ViewportInitialize(Viewport* viewport, glm::ivec2 size, glm::ivec2 position){
    viewport->size = size;
    viewport->position = position;

    glGenVertexArrays(1, &viewport->renderable.vao);
    glGenBuffers(1, &viewport->renderable.vbo);
    GLBufferData(&viewport->renderable, VERTEX_BUFFER, GL_DYNAMIC_DRAW, nullptr, QUAD_ARRAY_SIZE*sizeof(float), sizeof(float));
    GLAddAttribute(&viewport->renderable, GL_FLOAT, 3, 5*sizeof(float), 0);
    GLAddAttribute(&viewport->renderable, GL_FLOAT, 2, 5*sizeof(float), 3*sizeof(float));

    ViewportCreateQuad(viewport);

    FramebufferInitialize(&viewport->framebuffer, viewport->size.x, viewport->size.y);

    viewport->shader = {};
    ShaderCreate(&viewport->shader, "framebuffer_vertex.glsl", "framebuffer_fragment.glsl");
}

void ViewportFramebufferBind(Viewport* viewport){
    FramebufferBind(viewport->framebuffer.id);
}

void ViewportFramebufferUnbind(Viewport* viewport){
    FramebufferBind(0);
}

void ViewportUpdate(Viewport* viewport, glm::ivec2 size, glm::ivec2 position){
    viewport->size = size;
    viewport->position = position;

    ViewportCreateQuad(viewport);
    FramebufferResize(&viewport->framebuffer, viewport->size.x, viewport->size.y);
}

void ViewportDraw(Viewport* viewport){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);  // Make sure it draws over everything
    
    ShaderUse(&viewport->shader);
    TextureUse(&viewport->framebuffer.texture, GL_TEXTURE0);
    
    glm::mat4 ortho = glm::ortho(0.0f, (float)viewport->size.x, 0.0f, (float)viewport->size.y);
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(ortho));

    GLDraw(&viewport->renderable, DRAW_ARRAY, GL_TRIANGLES, 6);

    glEnable(GL_DEPTH_TEST);  // Restore for next frame
    glDisable(GL_BLEND);
}



#endif