#ifndef SHADER
#define SHADER

#include "common.cpp"

struct Shader{
    uint32_t programID;
};

void ShaderCheckError(uint32_t id, std::string type){
    int32_t success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(id, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n ------ \n";
        }
    }
    else
    {
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n ------ \n";
        }
    }
}

std::string ShaderLoadFile(std::string filePath){
    std::string shaderCode;
    std::ifstream shaderFile;
    
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        shaderFile.open(filePath);
        std::stringstream shaderStream;
        // read file's buffer contents into streams
        shaderStream << shaderFile.rdbuf();
        // close file handlers
        shaderFile.close();
        // convert stream into string
        shaderCode = shaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << "IN::" << filePath << std::endl;
    }   
    return shaderCode;
}

uint32_t ShaderGenerate(std::string path, uint32_t type){
    uint32_t shader = glCreateShader(type);
    std::string shaderCode = ShaderLoadFile(path);

    const char* shaderSource = shaderCode.c_str();
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    ShaderCheckError(shader, "SHADER");

    return shader;
}

void ShaderCreate(Shader* shader, std::string vertexPath, std::string fragmentPath){
    int vertexShader = ShaderGenerate("resources/shaders/" + vertexPath, GL_VERTEX_SHADER);
    int fragmentShader = ShaderGenerate("resources/shaders/" + fragmentPath, GL_FRAGMENT_SHADER);

    shader->programID = glCreateProgram();

    glAttachShader(shader->programID, vertexShader);
    glAttachShader(shader->programID, fragmentShader);
    glLinkProgram(shader->programID);
    ShaderCheckError(shader->programID, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


#endif