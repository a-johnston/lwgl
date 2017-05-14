#ifndef lwgl_shader_cpp

#define lwgl_shader_cpp

#include "lwgl.hpp"

static GLuint make_shader_component(GLenum type, std::string filename) {
    std::string source = lwgl::io::read_file(filename);
    const char *c_src = source.c_str();

    GLuint shader = glCreateShader(type);
    GLint shader_ok;

    glShaderSource(shader, 1, (const GLchar**) &c_src, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);

    if (!shader_ok) {
        std::cerr << "Failed to compile " << filename << "\n\n";
        lwgl::print_gl_log(shader, glGetShaderiv, glGetShaderInfoLog);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static GLuint make_shader_program(GLuint vertex, GLuint fragment) {
    GLint program_ok;
    GLuint program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);

    if (!program_ok) {
        std::cerr << "Failed to link shader program\n\n";
        lwgl::print_gl_log(program, glGetProgramiv, glGetProgramInfoLog);

        glDeleteProgram(program);
        return 0;
    }

    return program;
}

lwgl::Shader::Shader(std::string vertex, std::string fragment) {
    this->vert = make_shader_component(GL_VERTEX_SHADER, vertex);
    this->frag = make_shader_component(GL_FRAGMENT_SHADER, fragment);
    this->prog = make_shader_program(this->vert, this->frag);
}

lwgl::Shader::~Shader() {
    glDeleteShader(this->vert);
    glDeleteShader(this->frag);
    glDeleteProgram(this->prog);
}

int lwgl::Shader::map_attribute(LWGL_VERTEX_ATTR mesh_attr, std::string shader_attr) {
    int temp = glGetAttribLocation(this->prog, shader_attr.c_str());

    if (temp == -1) {
        std::cerr << "Warning: " << shader_attr << " does not map to a valid shader attribute";
        return temp;
    }

    this->attr_handles[mesh_attr] = (GLuint) temp;
    return temp + 1;
}

static void pass_gl_matrix_4fv(lwgl::shader_uniform uniform, glm::mat4 matrix) {
    glUniformMatrix4fv(uniform.handle, uniform.count, GL_FALSE, (const GLfloat*) &matrix);
}

static void pass_gl_texture_2d(lwgl::shader_uniform uniform, GLuint texId) {
    glActiveTexture(GL_TEXTURE0 + uniform.count);
    glBindTexture(GL_TEXTURE_2D, texId);
    glUniform1i(uniform.handle, uniform.count);
}

static void pass_gl_vector_3fv(lwgl::shader_uniform uniform, glm::vec3 vec) {
    glUniform3fv(uniform.handle, uniform.count, (const GLfloat*) &vec);
}

static void pass_gl_vector_4fv(lwgl::shader_uniform uniform, glm::vec4 vec) {
    glUniform4fv(uniform.handle, uniform.count, (const GLfloat*) &vec);
}

int lwgl::Shader::map_uniform(std::string handle, ShaderUniformType type, int count) {
    GLint handle_loc = glGetUniformLocation(this->prog, handle.c_str());

    if (handle_loc == -1) {
        std::cerr << "Warning: " << handle << " does not map to a valid shader uniform";
        return 0;
    }

    this->uniforms[handle] = (shader_uniform) {
        type,
        handle_loc,
        count
    };

    return 1;
}

int lwgl::Shader::set_uniform(std::string handle, ...) {
    if (this->uniforms.count(handle) == 0) {
        return 0;
    }

    shader_uniform uniform = this->uniforms[handle];

    va_list ap;
    va_start(ap, handle);

    glUseProgram(this->prog);
    switch (uniform.type) {
        case MATRIX_4FV:
            pass_gl_matrix_4fv(uniform, va_arg(ap, glm::mat4));
            break;
        case TEXTURE_2D:
            pass_gl_texture_2d(uniform, va_arg(ap, GLuint));
            break;
        case VECTOR_3FV:
            pass_gl_vector_3fv(uniform, va_arg(ap, glm::vec3));
            break;
        case VECTOR_4FV:
            pass_gl_vector_4fv(uniform, va_arg(ap, glm::vec4));
            break;
    }

    va_end(ap);

    return 1;
}

#endif
