#ifndef FRAMEBUFFER
#define FRAMEBUFFER

#include "common.cpp"
#include "texture.cpp"

void GLFramebufferBind(uint32_t framebufferID){
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
}

class Framebuffer {
public:
    uint32_t m_ID;
    uint32_t m_textureID;
    uint32_t m_depthRenderbuffer;
    uint32_t m_width;
    uint32_t m_height;
    bool m_hasDepthTest = false;

    void Initialize(uint32_t width, uint32_t height, bool depthTesting = false){
        m_width = width;
        m_height = height;
        m_hasDepthTest = depthTesting;

        glGenFramebuffers(1, &m_ID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); 
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureID, 0);
        
        if(m_hasDepthTest){
            glGenRenderbuffers(1, &m_depthRenderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);
        }

        // Check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "ERROR::FRAMEBUFFER:: HUD Framebuffer is not complete!" << std::endl;
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    
    void Resize(uint32_t newWidth, uint32_t newHeight){
        m_width = newWidth;
        m_height = newHeight;
        
        // Update the texture size
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        // Resize depth renderbuffer
        if(m_hasDepthTest){
            glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
        }
        
        // Reattach the texture to the framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureID, 0);

        // Check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR::FRAMEBUFFER:: HUD Framebuffer is not complete after resizing!" << std::endl;
        }

        // Unbind framebuffer and texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
};


#endif