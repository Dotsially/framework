#ifndef HUD
#define HUD

#include "common.cpp"
#include "math.cpp"
#include "input.cpp"
#include "window.cpp"
#include "render.cpp"
#include "framebuffer.cpp"
#include "viewport.cpp"
#include "bitmap.cpp"

struct HudItem;

struct HudQuad {
    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 uvOffset; // (x, y)
    glm::vec2 uvSize;   // (w, h) normalized
    glm::vec4 color;
    glm::vec4 marginsPixels;
    glm::vec4 marginsScreens;
    HudItem* parent = nullptr;
    int32_t layer = 0;
};

struct HudItem{
    std::vector<HudQuad*> quads;
    glm::vec2 size;
    glm::vec2 position;

    bool isHovered = false;
    bool isPressed = false;
    bool isHeldDown = false;
};

class Hud{
public:
    #define MAX_HUD_QUADS 1000
    #define VERTICES_PER_QUAD 6
    #define FLOATS_PER_VERTEX 9
    #define VERTEX_BUFFER_SIZE (MAX_HUD_QUADS * VERTICES_PER_QUAD * FLOATS_PER_VERTEX)

    Viewport m_viewport;
    uint32_t m_width;
    uint32_t m_height;

    std::vector<HudQuad> m_quads;
    std::vector<HudItem> m_hudItems;
    std::vector<float> m_vertexBuffer; // CPU-side vertex data
    GLRenderObject m_batch;
    Shader m_batchShader;

    void Initialize(){
        Window& window = Window::Instance();
        
        m_width = window.m_width;
        m_height = window.m_height;
        m_vertexBuffer.resize(VERTEX_BUFFER_SIZE);

        m_viewport.Initialize({m_width, m_height}, {0, 0});

        glGenVertexArrays(1, &m_batch.vao);
        glGenBuffers(1, &m_batch.vbo);
        GLBufferData(&m_batch, VERTEX_BUFFER, GL_DYNAMIC_DRAW, nullptr, VERTEX_BUFFER_SIZE, sizeof(float)); 
        GLAddAttribute(&m_batch, GL_FLOAT, 3, 9 * sizeof(float), 0);              // position
        GLAddAttribute(&m_batch, GL_FLOAT, 2, 9 * sizeof(float), 3 * sizeof(float)); // texcoord
        GLAddAttribute(&m_batch, GL_FLOAT, 4, 9 * sizeof(float), 5 * sizeof(float)); // color

        m_batchShader = {};
        m_batchShader.Create("hud/hud_batch_vertex.glsl", "hud/hud_batch_fragment.glsl");
    }

    void ResizeFramebuffer(uint32_t newWidth, uint32_t newHeight) {
        m_width = newWidth;
        m_height = newHeight;

        m_viewport.Update({m_width, m_height}, {0,0});
    }

    HudItem* AddItemQuad(glm::vec2 position, glm::vec2 size, glm::vec2 uvOffset, glm::vec2 uvSize, glm::vec4 color, int32_t layer = 0) {
        //Create hud item
        m_hudItems.emplace_back();
        HudItem* item = &m_hudItems.back();

        //Create hud quad
        m_quads.emplace_back(HudQuad{position, size, uvOffset, uvSize, color, {}, {}, item, layer});
        HudQuad* quad = &m_quads.back();

        item->quads.push_back(quad);
        item->position = position;
        item->size = size;

        return item;
    }

    // marginPixels: amount of pixels in the texture for 9-slice
    // marginScreen: size of margin texture on screen
    HudItem* AddItemNineSlice(glm::vec2 position, glm::vec2 size, glm::vec2 uvOffset, glm::vec2 uvSize, glm::vec4 color, glm::vec4 marginsPixels, glm::vec4 marginsScreen, uint32_t textureAtlasSize, int32_t layer = 0){
        m_hudItems.emplace_back();
        HudItem* item = &m_hudItems.back();

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

        auto addQuad = [&](glm::vec2 pos, glm::vec2 size, glm::vec2 uvStart, glm::vec2 uvSize, glm::vec4 color, glm::vec4 marginsPixels, glm::vec4 marginsScreen, int32_t layer) {
            m_quads.emplace_back(HudQuad{pos, size, uvStart, uvSize, color, marginsPixels, marginsScreen, item, layer});
            HudQuad* quad = &m_quads.back();
            item->quads.push_back(quad);
        };

        // Corners
        addQuad({x, y}, {left, bottom}, {u0, v0}, {u1 - u0, v1 - v0}, color, marginsPixels, marginsScreen, layer);
        addQuad({x + w - right, y}, {right, bottom}, {u2, v0}, {u3 - u2, v1 - v0}, color, marginsPixels, marginsScreen, layer);
        addQuad({x, y + h - top}, {left, top}, {u0, v2}, {u1 - u0, v3 - v2}, color, marginsPixels, marginsScreen, layer);
        addQuad({x + w - right, y + h - top}, {right, top}, {u2, v2}, {u3 - u2, v3 - v2}, color, marginsPixels, marginsScreen, layer);

        // Edges
        addQuad({x + left, y}, {innerW, bottom}, {u1, v0}, {u2 - u1, v1 - v0}, color, marginsPixels, marginsScreen, layer);
        addQuad({x + left, y + h - top}, {innerW, top}, {u1, v2}, {u2 - u1, v3 - v2}, color, marginsPixels, marginsScreen, layer);
        addQuad({x, y + bottom}, {left, innerH}, {u0, v1}, {u1 - u0, v2 - v1}, color, marginsPixels, marginsScreen, layer);
        addQuad({x + w - right, y + bottom}, {right, innerH}, {u2, v1}, {u3 - u2, v2 - v1}, color, marginsPixels, marginsScreen, layer);

        // Center
        addQuad({x + left, y + bottom}, {innerW, innerH}, {u1, v1}, {u2 - u1, v2 - v1}, color, marginsScreen, marginsPixels, layer);

        item->position = glm::vec2(x,y);
        item->size = glm::vec2(w, h);

        return item;
    }

    void UpdateNineSliceTexture(HudItem* item, glm::vec2 uvOffset, glm::vec2& uvSize, uint32_t& textureAtlasSize) {
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
    
    HudItem* RenderText(BitmapFont* font, const std::string& text, glm::vec2 position, glm::vec4 color, float scale = 1.0f, float spacing = 0.0f, float lineSpacing = 4.0f, int32_t layer = 0) {
        glm::vec2 cursor = position;
        glm::vec2 glyphSize = font->m_glyphs['A'].size * scale;

        float maxWidth = 0.0f;
        float maxHeight = glyphSize.y; // Start with height of first line

        m_hudItems.emplace_back();
        HudItem* item = &m_hudItems.back();

        for (size_t i = 0; i < text.size(); ++i) {
            char c = text[i];

            if (c == '\n') {
                cursor.x = position.x;
                cursor.y -= (glyphSize.y + lineSpacing);
                float totalHeight = position.y - cursor.y + glyphSize.y; // total height so far
                maxHeight = Max(maxHeight, totalHeight);
                continue;
            }

            auto it = font->m_glyphs.find(c);
            if (it == font->m_glyphs.end()) continue;

            const BitmapGlyph& glyph = it->second;

            m_quads.emplace_back(HudQuad{
                cursor,
                glyphSize,
                glyph.uvOffset,
                glyph.uvSize,
                color,
                {},
                {},
                item,
                layer
            });

            HudQuad* quad = &m_quads.back();
            item->quads.push_back(quad);

            cursor.x += glyphSize.x + spacing;
            maxWidth = Max(maxWidth, cursor.x - position.x);
        }

        item->position = glm::vec2(position.x, position.y - maxHeight + glyphSize.y);
        item->size = glm::vec2(maxWidth, maxHeight);

        return item;
    }

    
    void UpdateColor(HudItem* item, glm::vec4 color){
        for (HudQuad* quad : item->quads) {
            quad->color = color;
        }
    }

    void UpdateItemState(HudItem* item, InputState& input) {
        float mouseX = input.mousePosition.x;
        float mouseY = m_height - input.mousePosition.y; // Flip Y

        float x1 = item->position.x;
        float y1 = item->position.y;
        float x2 = x1 + item->size.x;
        float y2 = y1 + item->size.y;

        bool hovered =  mouseX >= x1 && mouseX <= x2 &&
                        mouseY >= y1 && mouseY <= y2;

        if (hovered) {
            item->isHovered = true;

            if (input.mouseButtonStates.leftIsPressed &&
                !input.mouseButtonStates.leftWasPressed)
                item->isPressed = true;

            if (input.mouseButtonStates.leftIsPressed)
                item->isHeldDown = true;
        }
    }


    
    //TODO change layer to depth buffer and let cpu sort. Increase max amount of quads for more expressive UIs
    void DrawToFrameBuffer(Texture& textureAtlas){
        std::sort(m_quads.begin(), m_quads.end(), [](const HudQuad& a, const HudQuad& b) {
            return a.layer < b.layer;
        });

        m_viewport.BindFramebuffer();
        glViewport(0, 0, m_width, m_height);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_batchShader.Use();
        TextureUse(textureAtlas.ID, GL_TEXTURE0);

        glm::mat4 ortho = glm::ortho(0.0f, (float)m_width, 0.0f, (float)m_height);
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(ortho));

        float* bufferPtr = m_vertexBuffer.data();
        size_t vertexIndex = 0;
        size_t quadCount = 0;

        for (const HudQuad& quad : m_quads) {
            if (quadCount >= MAX_HUD_QUADS) break;

            float x = quad.position.x;
            float y = quad.position.y;
            float w = quad.size.x;
            float h = quad.size.y;

            float u0 = quad.uvOffset.x;
            float v0 = quad.uvOffset.y;
            float u1 = u0 + quad.uvSize.x;
            float v1 = v0 + quad.uvSize.y;

            glm::vec4 color = quad.color;

            // 6 vertices × 5 floats each = 30 floats per quad
            float quadVertices[54] = {
                // position         // texcoord
                x,     y,     0.0f, u0, v0, color.r, color.g, color.b, color.a, // bottom-left
                x + w, y + h, 0.0f, u1, v1, color.r, color.g, color.b, color.a, // top-right
                x + w, y,     0.0f, u1, v0, color.r, color.g, color.b, color.a, // bottom-right

                x,     y,     0.0f, u0, v0, color.r, color.g, color.b, color.a, // bottom-left
                x,     y + h, 0.0f, u0, v1, color.r, color.g, color.b, color.a, // top-left
                x + w, y + h, 0.0f, u1, v1, color.r, color.g, color.b, color.a,// top-right
            };

            // Copy directly into the preallocated vector buffer
            for (int i = 0; i < 54; ++i) {
                bufferPtr[vertexIndex++] = quadVertices[i];
            }

            ++quadCount;
        }

        // Upload only the used part of the buffer
        size_t totalSize = vertexIndex * sizeof(float);

        glBindVertexArray(m_batch.vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_batch.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, totalSize, m_vertexBuffer.data());

        m_batchShader.Use();
        GLDraw(&m_batch, DRAW_ARRAY, GL_TRIANGLES, quadCount * VERTICES_PER_QUAD);    
        glBindVertexArray(0);

        //Restore state
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        m_viewport.UnbindFramebuffer();
        
        m_hudItems.clear();
        m_quads.clear();
    }

    void Draw(){
        m_viewport.Draw();
    }
};



#endif