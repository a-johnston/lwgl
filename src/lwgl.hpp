#ifndef lwgl_hpp

#define lwgl_hpp

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

namespace lwgl {

/*
 * mesh.cpp
 */

class Mesh {
    GLuint vert_vbo;
    GLuint tri_vbo;

    std::string format;

    std::vector<glm::vec4> verts;
    std::vector<glm::ivec3> tris;

    public:
        Mesh(std::string format);
        int add_vertex(std::string format, ...);
        int add_tri(glm::ivec3);
        int add_tri(int, int, int);
        void buffer();
        void unbuffer();
        void __print_debug();
    private:
        int get_attr_off(char a);
        int get_attr_id(char a, int vid);
        void set_attr(char a, int vid, glm::vec4 value);
};

/*
 * shader.cpp
 */

enum ShaderUniformType {
    MATRIX_4FV,
    VECTOR_3FV,
    VECTOR_4FV,
    TEXTURE_2D
};

typedef void (*_uniform_setter)(GLuint, int, void*);

typedef struct {
    GLuint handle;
    int count;
    _uniform_setter func;
} shader_uniform;

class Shader {
    GLuint vert, frag, prog;
    std::map<char, GLuint> attr_handles;
    std::vector<shader_uniform> uniforms;

    public:
        Shader(std::string vertex_filename, std::string fragment_filename);
        ~Shader();
        int map_attribute(char mesh_attr, std::string shader_attr);
        shader_uniform *map_uniform(ShaderUniformType type, std::string handle, int count);
};

/*
 * render.cpp
 */

GLFWwindow *make_window(int width, int height, std::string title);
void add_key_callback(GLFWkeyfun f);

/*
 * io.cpp
 */

namespace io {
    std::string read_file(std::string filename);
}

}

#endif
