#ifndef FRAMEBUFFER
#define FRAMEBUFFER

#include "common.cpp"
#include "texture.cpp"


struct Framebuffer {
    uint32_t id;
    Texture texture;
    uint32_t depthRenderbuffer;
    uint32_t width;
    uint32_t height;
    bool hasDepthTest = false;
};


void FramebufferInitialize(Framebuffer* framebuffer, uint32_t width, uint32_t height, bool depthTesting = false){
    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->hasDepthTest = depthTesting;

    glGenFramebuffers(1, &framebuffer->id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);

    glGenTextures(1, &framebuffer->texture.textureID);
    glBindTexture(GL_TEXTURE_2D, framebuffer->texture.textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebuffer->width, framebuffer->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->texture.textureID, 0);
    
    if(framebuffer->hasDepthTest){
        glGenRenderbuffers(1, &framebuffer->depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, framebuffer->width, framebuffer->height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer->depthRenderbuffer);
    }

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: HUD Framebuffer is not complete!" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


void FramebufferResize(Framebuffer* framebuffer, uint32_t newWidth, uint32_t newHeight){
    framebuffer->width = newWidth;
    framebuffer->height = newHeight;
    
    // Update the texture size
    glBindTexture(GL_TEXTURE_2D, framebuffer->texture.textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebuffer->width, framebuffer->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Resize depth renderbuffer
    if(framebuffer->hasDepthTest){
        glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, framebuffer->width, framebuffer->height);
    }
    
    // Reattach the texture to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->texture.textureID, 0);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: HUD Framebuffer is not complete after resizing!" << std::endl;
    }

    // Unbind framebuffer and texture
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


void FramebufferBind(uint32_t framebufferID){
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
}


#endif