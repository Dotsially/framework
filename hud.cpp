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

struct HudItem;

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
    glm::vec4 marginsPixels;
    glm::vec4 marginsScreens;
    HudItem* parent = nullptr;
};

struct HudItem{
    std::vector<HudQuad*> quads;
    
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

    std::vector<HudItem> hudItems;
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

HudItem* HudAddItemQuad(Hud* hud, glm::vec2 position, glm::vec2 size, glm::vec2 uvOffset, glm::vec2 uvSize) {
    //Create hud item
    hud->hudItems.emplace_back();
    HudItem* item = &hud->hudItems.back();

    //Create hud quad
    hud->quads.emplace_back(HudQuad{position, size, uvOffset, uvSize});
    HudQuad* quad = &hud->quads.back();
    quad->parent = item;

    item->quads.push_back(quad);

    return item;
}

HudItem* HudAddItemNineSlice(Hud* hud, glm::vec2 position, glm::vec2 size, glm::vec2 uvOffset, glm::vec2 uvSize, glm::vec4 marginsPixels, glm::vec4 marginsScreen, uint32_t textureAtlasSize){
    hud->hudItems.emplace_back();
    HudItem* item = &hud->hudItems.back();

    float left   = marginsScreen.x;
    float right  = marginsScreen.y;
    float top    = marginsScreen.z;
    float bottom = marginsScreen.w;

    float x = position.x;
    float y = position.y;
    float w = size.x;
    float h = size.y;

    // Convert pixel margins to UV margins
    glm::vec4 uvMargins = {
        marginsPixels.x / (float)textureAtlasSize,
        marginsPixels.y / (float)textureAtlasSize,
        marginsPixels.z / (float)textureAtlasSize,
        marginsPixels.w / (float)textureAtlasSize
    };

    glm::vec2 uvMin = uvOffset;
    glm::vec2 uvMax = uvOffset + uvSize;

    float u0 = uvMin.x;
    float u1 = u0 + uvMargins.x;
    float u2 = uvMax.x - uvMargins.y;
    float u3 = uvMax.x;

    float v0 = uvMin.y;
    float v1 = v0 + uvMargins.w;
    float v2 = uvMax.y - uvMargins.z;
    float v3 = uvMax.y;

    float innerW = w - left - right;
    float innerH = h - top - bottom;

    auto addQuad = [&](glm::vec2 pos, glm::vec2 size, glm::vec2 uvStart, glm::vec2 uvSize, glm::vec4 marginsPixels, glm::vec4 marginsScreen) {
        hud->quads.emplace_back(HudQuad{pos, size, uvStart, uvSize, marginsPixels, marginsScreen});
        HudQuad* quad = &hud->quads.back();
        quad->parent = item;
        item->quads.push_back(quad);
    };

    // Corners
    addQuad({x, y}, {left, bottom}, {u0, v0}, {u1 - u0, v1 - v0}, marginsPixels, marginsScreen);
    addQuad({x + w - right, y}, {right, bottom}, {u2, v0}, {u3 - u2, v1 - v0}, marginsPixels, marginsScreen);
    addQuad({x, y + h - top}, {left, top}, {u0, v2}, {u1 - u0, v3 - v2}, marginsPixels, marginsScreen);
    addQuad({x + w - right, y + h - top}, {right, top}, {u2, v2}, {u3 - u2, v3 - v2}, marginsPixels, marginsScreen);

    // Edges
    addQuad({x + left, y}, {innerW, bottom}, {u1, v0}, {u2 - u1, v1 - v0}, marginsPixels, marginsScreen);
    addQuad({x + left, y + h - top}, {innerW, top}, {u1, v2}, {u2 - u1, v3 - v2}, marginsPixels, marginsScreen);
    addQuad({x, y + bottom}, {left, innerH}, {u0, v1}, {u1 - u0, v2 - v1}, marginsPixels, marginsScreen);
    addQuad({x + w - right, y + bottom}, {right, innerH}, {u2, v1}, {u3 - u2, v2 - v1}, marginsPixels, marginsScreen);

    // Center
    addQuad({x + left, y + bottom}, {innerW, innerH}, {u1, v1}, {u2 - u1, v2 - v1}, marginsScreen, marginsPixels);

    return item;
}

void HudUpdateNineSliceTexture(HudItem* item, glm::vec2 uvOffset, glm::vec2& uvSize, uint32_t& textureAtlasSize) {
    uint32_t count = 0;
    for (HudQuad* quad : item->quads) {
        // Convert pixel margins to UV margins (left, top, right, bottom)
        glm::vec4 uvMargins = {
            quad->marginsPixels.x / (float)textureAtlasSize, // left
            quad->marginsPixels.y / (float)textureAtlasSize, // top
            quad->marginsPixels.z / (float)textureAtlasSize, // right
            quad->marginsPixels.w / (float)textureAtlasSize  // bottom
        };

        glm::vec2 uvMin = uvOffset;
        glm::vec2 uvMax = uvOffset + uvSize;

        // U coords
        float u0 = uvMin.x;
        float u1 = u0 + uvMargins.x;
        float u2 = uvMax.x - uvMargins.z; // right margin is z
        float u3 = uvMax.x;

        // V coords
        float v0 = uvMin.y;
        float v1 = v0 + uvMargins.y; // top margin is y
        float v2 = uvMax.y - uvMargins.w; // bottom margin is w
        float v3 = uvMax.y;

        switch (count) {
            case 0: quad->uvOffset = { u0, v0 }; quad->uvSize = { u1 - u0, v1 - v0 }; break; // Bottom-left
            case 1: quad->uvOffset = { u2, v0 }; quad->uvSize = { u3 - u2, v1 - v0 }; break; // Bottom-right
            case 2: quad->uvOffset = { u0, v2 }; quad->uvSize = { u1 - u0, v3 - v2 }; break; // Top-left
            case 3: quad->uvOffset = { u2, v2 }; quad->uvSize = { u3 - u2, v3 - v2 }; break; // Top-right
            case 4: quad->uvOffset = { u1, v0 }; quad->uvSize = { u2 - u1, v1 - v0 }; break; // Bottom edge
            case 5: quad->uvOffset = { u1, v2 }; quad->uvSize = { u2 - u1, v3 - v2 }; break; // Top edge
            case 6: quad->uvOffset = { u0, v1 }; quad->uvSize = { u1 - u0, v2 - v1 }; break; // Left edge
            case 7: quad->uvOffset = { u2, v1 }; quad->uvSize = { u3 - u2, v2 - v1 }; break; // Right edge
            case 8: quad->uvOffset = { u1, v1 }; quad->uvSize = { u2 - u1, v2 - v1 }; break; // Center
            default: break;
        }
        
        count++;
    }
}

void HudUpdateItemState(Hud* hud, HudItem* item, InputState& input) {
    float mouseX = input.mousePosition.x;
    float mouseY = hud->height - input.mousePosition.y; // Flip Y

    for (HudQuad* quad : item->quads) {
        float x1 = quad->position.x;
        float y1 = quad->position.y;
        float x2 = x1 + quad->size.x;
        float y2 = y1 + quad->size.y;

        bool hovered = mouseX >= x1 && mouseX <= x2 &&
                       mouseY >= y1 && mouseY <= y2;

        if (hovered) {
            item->isHovered = true;

            if (input.mouseButtonStates.leftIsPressed &&
                !input.mouseButtonStates.leftWasPressed)
                item->isPressed = true;

            if (input.mouseButtonStates.leftIsPressed)
                item->isHeldDown = true;

            break; // No need to continue checking other quads if one is hovered
        }
    }
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
    
    hud->hudItems.clear();
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