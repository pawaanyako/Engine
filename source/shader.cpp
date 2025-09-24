#include <fstream>
#include <iostream>
#include <sstream>

#include <shader.h>

Shader::Shader(const std::vector<std::string>& shader_filepaths) {
    std::vector<GLuint> shaders;
    shaders.reserve(shader_filepaths.size());

    for (const std::string& filepath : shader_filepaths) {
        GLenum type = detect_shader_type(filepath);
        const std::string code = read_shader_source(filepath);
        GLuint shader_id = compile_shader(type, code);
        shaders.push_back(shader_id);
    }

    program_id_ = glCreateProgram();
    for (GLuint shader_id : shaders) {
        glAttachShader(program_id_, shader_id);
    }
    glLinkProgram(program_id_);

    check_link_errors(program_id_);

    for (GLuint shader_id : shaders) {
        glDeleteShader(shader_id);
    }
}

Shader::~Shader() {
    if (program_id_ != 0) {
        glDeleteProgram(program_id_);
    }
}

void Shader::use() const {
    glUseProgram(program_id_);
}

GLenum Shader::detect_shader_type(const std::string& filepath) {
    if (filepath.find(".vert") != std::string::npos)
        return GL_VERTEX_SHADER;
    if (filepath.find(".frag") != std::string::npos)
        return GL_FRAGMENT_SHADER;
    if (filepath.find(".geom") != std::string::npos)
        return GL_GEOMETRY_SHADER;
    throw std::runtime_error("Unknown shader file extension: " + filepath);
}

const std::string Shader::read_shader_source(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) {
        throw std::runtime_error("Failed to open shader file: " + filepath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

GLuint Shader::compile_shader(GLenum type, const std::string& source) {
    GLuint shader_id = glCreateShader(type);
    const GLchar* c_source = source.c_str();
    glShaderSource(shader_id, 1, &c_source, nullptr);
    glCompileShader(shader_id);

    check_compile_errors(shader_id);

    return shader_id;
}

void Shader::check_compile_errors(GLuint shader_id) {
    GLint success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar info_log[INFO_LOG_MAX_LENGTH];
        glGetShaderInfoLog(shader_id, INFO_LOG_MAX_LENGTH, nullptr, info_log);
        throw std::runtime_error("Shader compilation error: " + std::string(info_log));
    }
}

void Shader::check_link_errors(GLuint program_id) {
    GLint success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar info_log[INFO_LOG_MAX_LENGTH];
        glGetProgramInfoLog(program_id, INFO_LOG_MAX_LENGTH, nullptr, info_log);
        throw std::runtime_error("Program linking error: " + std::string(info_log));
    }
}
