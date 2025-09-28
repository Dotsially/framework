#ifndef TEXTURE_ARRAY
#define TEXTURE_ARRAY

#include "common.cpp"
#include "common_graphics.cpp"

struct TextureArray{
    uint32_t m_textureID;
    uint32_t m_textureUnitCount;

    /*
    * textureSize is the size of each texture in the array. Must be constant.
    * arrayDepth how many textures the array holds.
    */
    void Initialize(int textureSize, int arrayDepth){
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, textureSize, textureSize, arrayDepth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  

        m_textureUnitCount = 0;
    }

    void Add(std::string& fileName){
        std::string filePath = "resources/textures/" + fileName;
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID); 

        // load and generate the textures
        int32_t width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);  
        uint8_t* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 4);
        
        if (data == NULL){
            std::cout << "Failed to load texture" << std::endl;
            return;
        }
        
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_textureUnitCount, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        stbi_image_free(data);

        m_textureUnitCount += 1;
    }

    void Use(uint32_t textureUnit){
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID); 
    }
    
};

#endif