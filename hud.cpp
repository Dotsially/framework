#ifndef HUD
#define HUD

#include "common.cpp"
#include "input.cpp"
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
    glm::vec2 uvOffset; // (x, y)
    glm::vec2 uvSize;   // (w, h) normalized
    glm::vec4 margins;
};

struct HudQuadState{
    HudQuad* quad;
    bool isHovered = false;
    bool isPressed = false;
    bool isHeldDown = false;
};

struct Hud{
    GLRenderObject renderable;
    Shader shader;
    Texture texture;
    uint32_t framebufferID;
    int32_t width;
    int32_t height;

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

void HudResizeFramebuffer(Hud* hud, int32_t newWidth, int32_t newHeight) {
    hud->width = newWidth;
    hud->height = newHeight;

    glBindTexture(GL_TEXTURE_2D, hud->texture.textureID);

    // Update the texture size
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Reattach the texture to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, hud->framebufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hud->texture.textureID, 0);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: HUD Framebuffer is not complete after resizing!" << std::endl;
    }

    // Unbind framebuffer and texture
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void HudAddQuad(Hud* hud, glm::vec2 position, glm::vec2 size, glm::vec2 uvOffset, glm::vec2 uvSize) {
    HudQuad quad = { position, size, uvOffset, uvSize };
    hud->quads.push_back(quad);
}

void HudAddNineSliceQuad(Hud* hud, glm::vec2 position, glm::vec2 size, glm::vec2 uvOffset, glm::vec2 uvSize, glm::vec4 margins) {
    float left = margins.x;
    float right = margins.y;
    float top = margins.z;
    float bottom = margins.w;

    float x = position.x;
    float y = position.y;
    float w = size.x;
    float h = size.y;

    float u0 = uvOffset.x;
    float v0 = uvOffset.y;
    float u1 = u0 + uvSize.x;
    float v1 = v0 + uvSize.y;

    // Corners
    HudAddQuad(hud, {x, y}, {left, bottom}, {u0, v0}, {left / w, bottom / h}); // Bottom-left
    HudAddQuad(hud, {x + w - right, y}, {right, bottom}, {u1 - right / w, v0}, {right / w, bottom / h}); // Bottom-right
    HudAddQuad(hud, {x, y + h - top}, {left, top}, {u0, v1 - top / h}, {left / w, top / h}); // Top-left
    HudAddQuad(hud, {x + w - right, y + h - top}, {right, top}, {u1 - right / w, v1 - top / h}, {right / w, top / h}); // Top-right

    // Edges
    HudAddQuad(hud, {x + left, y}, {w - left - right, bottom}, {u0 + left / w, v0}, {(w - left - right) / w, bottom / h}); // Bottom edge
    HudAddQuad(hud, {x + left, y + h - top}, {w - left - right, top}, {u0 + left / w, v1 - top / h}, {(w - left - right) / w, top / h}); // Top edge
    HudAddQuad(hud, {x, y + bottom}, {left, h - top - bottom}, {u0, v0 + bottom / h}, {left / w, (h - top - bottom) / h}); // Left edge
    HudAddQuad(hud, {x + w - right, y + bottom}, {right, h - top - bottom}, {u1 - right / w, v0 + bottom / h}, {right / w, (h - top - bottom) / h}); // Right edge

    // Center
    HudAddQuad(hud, {x + left, y + bottom}, {w - left - right, h - top - bottom}, {u0 + left / w, v0 + bottom / h}, {(w - left - right) / w, (h - top - bottom) / h});
}

HudQuadState HudQuadGetState(Hud* hud, InputState& input, size_t quadIndex) {
    HudQuadState quadState = {};

    if (quadIndex >= hud->quads.size()) {
        return quadState;
    }

    HudQuad& quad = hud->quads[quadIndex];
    quadState.quad = &quad;

    float mouseX = input.mousePosition.x;
    float mouseY = hud->height - input.mousePosition.y;

    float x1 = quad.position.x;
    float y1 = quad.position.y;
    float x2 = x1 + quad.size.x;
    float y2 = y1 + quad.size.y;

    quadState.isHovered =   mouseX >= x1 && mouseX <= x2 &&
                            mouseY >= y1 && mouseY <= y2;

    quadState.isPressed =   quadState.isHovered && 
                            input.mouseButtonStates.leftIsPressed &&
                            !input.mouseButtonStates.leftWasPressed;

    quadState.isHeldDown =  quadState.isHovered && 
                            input.mouseButtonStates.leftIsPressed;

    return quadState;
}

void HudDrawToFrameBuffer(Hud* hud, Texture& textureAtlas){
    glBindFramebuffer(GL_FRAMEBUFFER, hud->framebufferID);
    glViewport(0, 0, hud->width, hud->height);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    ShaderUse(&hud->batchShader);
    TextureUse(&textureAtlas, GL_TEXTURE0);

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

        float u0 = quad.uvOffset.x;
        float v0 = quad.uvOffset.y;
        float u1 = u0 + quad.uvSize.x;
        float v1 = v0 + quad.uvSize.y;

        float quadVerts[VERTICES_PER_QUAD * FLOATS_PER_VERTEX] = {
            // positions     // texcoords
            x,     y,     0.0f,  u0, v0, // bottom-left
            x + w, y + h, 0.0f,  u1, v1, // top-right
            x + w, y,     0.0f,  u1, v0, // bottom-right

            x,     y,     0.0f,  u0, v0, // bottom-left
            x,     y + h, 0.0f,  u0, v1, // top-left
            x + w, y + h, 0.0f,  u1, v1  // top-right
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);  // Make sure it draws over everything
    
    ShaderUse(&hud->shader);
    TextureUse(&hud->texture, GL_TEXTURE0);
    GLDraw(&hud->renderable, DRAW_ARRAY, GL_TRIANGLES, 6);

    glEnable(GL_DEPTH_TEST);  // Restore for next frame
    glDisable(GL_BLEND);
}



#endif