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
        std::cerr << "Failed to compile " << filename << 'n';
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
        std::cerr << "Failed to link shader program\n";
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

lwgl::Shader::Shader(std::string vertex, std::string fragment) {
    this->vert = make_shader_component(GL_VERTEX_SHADER, vertex);
    this->frag = make_shader_component(GL_FRAGMENT_SHADER, fragment);
    this->prog = make_shader_program(this->vert, this->frag);


    this->attr_handles = std::map<char, GLuint>();
    this->uniforms = std::vector<shader_uniform>();
}

lwgl::Shader::~Shader() {
    glDeleteShader(this->vert);
    glDeleteShader(this->frag);
    glDeleteProgram(this->prog);
}

int lwgl::Shader::map_attribute(char mesh_attr, std::string shader_attr) {
    int temp = glGetAttribLocation(this->prog, shader_attr.c_str());

    if (temp >= 0) {
        this->attr_handles[mesh_attr] = (GLuint) temp;
        return temp + 1;
    }

    return 0;
}

static void pass_gl_matrix_4fv(GLuint handle, int count, void *matrix) {
    glUniformMatrix4fv(handle, count, GL_FALSE, (const GLfloat*) matrix);
}

static void pass_gl_texture_2d(GLuint handle, int texUnit, void *texId) {
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, *(GLuint*) texId);
    glUniform1i(handle, texUnit);
}

static void pass_gl_vector_3fv(GLuint handle, int count, void *vec) {
    glUniform3fv(handle, count, (const GLfloat*) vec);
}

static void pass_gl_vector_4fv(GLuint handle, int count, void *vec) {
    glUniform4fv(handle, count, (const GLfloat*) vec);
}

lwgl::shader_uniform *lwgl::Shader::map_uniform(ShaderUniformType type, std::string handle, int count) {
    shader_uniform data;
    data.handle = glGetUniformLocation(this->prog, handle.c_str());
    data.count = count;
    data.func = NULL;

    switch (type) {
        case MATRIX_4FV:
            data.func = pass_gl_matrix_4fv;
            break;
        case TEXTURE_2D:
            data.func = pass_gl_texture_2d;
            break;
        case VECTOR_3FV:
            data.func = pass_gl_vector_3fv;
            break;
        case VECTOR_4FV:
            data.func = pass_gl_vector_4fv;
            break;
    }

    this->uniforms.push_back(data);
    return &this->uniforms.back();
}

#endif
