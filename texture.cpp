#ifndef TEXTURE
#define TEXTURE

#include "common.cpp"
#include "common_graphics.cpp"

struct Texture{
    uint32_t ID;
    uint32_t width;
    uint32_t height;
};

void TextureFromFile(Texture* texture, const std::string& fileName, bool billinearFilter = true){
    std::string filePath = "resources/textures/" + fileName;
    glGenTextures(1, &texture->ID);
    glBindTexture(GL_TEXTURE_2D, texture->ID); 


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    if(billinearFilter){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    }
    else{
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
    }

    // load and generate the textures
    int32_t width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);  
    uint8_t* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    
    if (data == NULL){
        std::cout << "Failed to load texture" << std::endl;
        return;
    }

    texture->width = width;
    texture->height = height;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

void TextureFromMemory(Texture* texture, uint8_t* textureData, uint32_t width, uint32_t height, bool billinearFilter = true){
    texture->width = width;
    texture->height = height;
    
    glGenTextures(1, &texture->ID);
    glBindTexture(GL_TEXTURE_2D, texture->ID); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if(billinearFilter){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
    }
    else{
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureUse(uint32_t textureID, uint32_t textureUnit){
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureID); 
}


#endif