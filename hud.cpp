#ifndef HUD
#define HUD

#include "common.cpp"
#include "window.cpp"
#include "render.cpp"
#include "shader.cpp"
#include "texture.cpp"

#define MAX_HUD_QUADS 1000
#define VERTICES_PER_QUAD 6
#define FLOATS_PER_VERTEX 5
#define VERTEX_BUFFER_SIZE (MAX_HUD_QUADS * VERTICES_PER_QUAD * FLOATS_PER_VERTEX)

float quadVertices[] = {
    // positions         // texCoords
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,

    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f
};

struct HudQuad {
    glm::vec2 position;
    glm::vec2 size;
};

struct Hud{
    GLRenderObject renderable;
    Shader shader;
    Texture texture;
    uint32_t framebufferID;
    uint32_t width;
    uint32_t height;

    std::vector<HudQuad> quads;
    float vertexBuffer[VERTEX_BUFFER_SIZE]; // CPU-side vertex data
    GLRenderObject batch;
    Shader batchShader;
};

void HudInitialize(Hud* hud, Window& window){
    hud->width = window.width;
    hud->height = window.height;

    glGenVertexArrays(1, &hud->renderable.vao);
    glGenBuffers(1, &hud->renderable.vbo);
    GLBufferData(&hud->renderable, VERTEX_BUFFER, GL_STATIC_DRAW, quadVertices, sizeof(quadVertices), sizeof(quadVertices[0]));
    GLAddAttribute(&hud->renderable, GL_FLOAT, 3, 5*sizeof(float), 0);
    GLAddAttribute(&hud->renderable, GL_FLOAT, 2, 5*sizeof(float), 3*sizeof(float));

    glGenFramebuffers(1, &hud->framebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, hud->framebufferID);

    glGenTextures(1, &hud->texture.textureID);
    glBindTexture(GL_TEXTURE_2D, hud->texture.textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hud->width, hud->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hud->texture.textureID, 0);
    
    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: HUD Framebuffer is not complete!" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenVertexArrays(1, &hud->batch.vao);
    glGenBuffers(1, &hud->batch.vbo);
    GLBufferData(&hud->batch, VERTEX_BUFFER, GL_DYNAMIC_DRAW, nullptr, VERTEX_BUFFER_SIZE, sizeof(float)); 
    GLAddAttribute(&hud->batch, GL_FLOAT, 3, 5*sizeof(float), 0);
    GLAddAttribute(&hud->batch, GL_FLOAT, 2, 5*sizeof(float), 3*sizeof(float));

    hud->shader = {};
    ShaderCreate(&hud->shader, "hud/hud_vertex.glsl", "hud/hud_fragment.glsl");
    hud->batchShader = {};
    ShaderCreate(&hud->batchShader, "hud/hud_batch_vertex.glsl", "hud/hud_batch_fragment.glsl");
}

void HudAddQuad(Hud* hud, glm::vec2 position, glm::vec2 size) {
    HudQuad quad = { position, size };
    hud->quads.push_back(quad);
}

void HudDrawToFrameBuffer(Hud* hud){
    glBindFramebuffer(GL_FRAMEBUFFER, hud->framebufferID);
    glViewport(0, 0, hud->width, hud->height);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    ShaderUse(&hud->batchShader);

    glm::mat4 ortho = glm::ortho(0.0f, (float)hud->width, 0.0f, (float)hud->height);
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(ortho));

    float* bufferPtr = hud->vertexBuffer;
    size_t quadCount = 0;

    for (const HudQuad& quad : hud->quads) {
        if (quadCount >= MAX_HUD_QUADS) break;

        float x = quad.position.x;
        float y = quad.position.y;
        float w = quad.size.x;
        float h = quad.size.y;

        float quadVerts[VERTICES_PER_QUAD * FLOATS_PER_VERTEX] = {
            // positions         // texcoords
            x,     y,     0.0f,  0.0f, 0.0f,  // bottom-left
            x + w, y + h, 0.0f,  1.0f, 1.0f,  // top-right
            x + w, y,     0.0f,  1.0f, 0.0f,  // bottom-right

            x,     y,     0.0f,  0.0f, 0.0f,  // bottom-left
            x,     y + h, 0.0f,  0.0f, 1.0f,  // top-left
            x + w, y + h, 0.0f,  1.0f, 1.0f   // top-right
        };

        memcpy(bufferPtr, quadVerts, sizeof(quadVerts));
        bufferPtr += sizeof(quadVerts) / sizeof(float);
        ++quadCount;
    }

    // Upload batched buffer
    glBindVertexArray(hud->batch.vao);
    size_t totalSize = quadCount * VERTICES_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, hud->batch.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, totalSize, hud->vertexBuffer);
    

    ShaderUse(&hud->batchShader);
    GLDraw(&hud->batch, DRAW_ARRAY, GL_TRIANGLES, quadCount * VERTICES_PER_QUAD);

    
    glBindVertexArray(0);

    // Restore state
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    hud->quads.clear();
}


void HudDraw(Hud* hud){
    glDisable(GL_DEPTH_TEST);  // Make sure it draws over everything
    
    ShaderUse(&hud->shader);
    TextureUse(&hud->texture, GL_TEXTURE0);
    GLDraw(&hud->renderable, DRAW_ARRAY, GL_TRIANGLES, 6);

    glEnable(GL_DEPTH_TEST);  // Restore for next frame
}



#endif