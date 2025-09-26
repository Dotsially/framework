#ifndef SHADER
#define SHADER

#include "common.cpp"
#include "common_graphics.cpp"

class Shader{
public:    
    uint32_t m_programID;
    
    void Create(std::string vertexPath, std::string fragmentPath){
        int vertexShader = Generate("resources/shaders/" + vertexPath, GL_VERTEX_SHADER);
        int fragmentShader = Generate("resources/shaders/" + fragmentPath, GL_FRAGMENT_SHADER);

        m_programID = glCreateProgram();

        glAttachShader(m_programID, vertexShader);
        glAttachShader(m_programID, fragmentShader);
        glLinkProgram(m_programID);
        CheckError(m_programID, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void Use(){
        glUseProgram(m_programID);
    }

private:
    void CheckError(uint32_t id, std::string type){
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

    std::string LoadFile(std::string filePath){
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

    uint32_t Generate(std::string path, uint32_t type){
        uint32_t shader = glCreateShader(type);
        std::string shaderCode = LoadFile(path);

        const char* shaderSource = shaderCode.c_str();
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);
        CheckError(shader, "SHADER");

        return shader;
    }
};


#endif