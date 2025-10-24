#ifndef SHADER_H_
#define SHADER_H_
#define INFO_LOG_MAX_LENGTH 1024

#include <config.h>

class Shader {
public:
    Shader();
    Shader(const std::vector<std::string>& shader_filepaths);
    ~Shader();

    const GLuint get_program_id() const;
    void use() const;

private:
    GLuint program_id_;

    static GLenum detect_shader_type(const std::string& path);
    static const std::string read_shader_source(const std::string& path);
    static GLuint compile_shader(GLenum type, const std::string& source);
    static void check_compile_errors(GLuint shader_id);
    static void check_link_errors(GLuint program_id);
};
#endif
