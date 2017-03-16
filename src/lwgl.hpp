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

    friend class Mesh;

    public:
        Shader(std::string vertex_filename, std::string fragment_filename);
        ~Shader();
        int map_attribute(char mesh_attr, std::string shader_attr);
        shader_uniform *map_uniform(ShaderUniformType type, std::string handle, int count);
};

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
        void draw(Shader shader);
        void __print_debug();
    private:
        int get_attr_off(char a);
        int get_attr_id(char a, int vid);
        void set_attr(char a, int vid, glm::vec4 value);
        void bind_to_shader(Shader shader);
        void draw_mesh_tris();
        void unbind_from_shader(Shader shader);
};

/*
 * game.cpp
 */

class Actor {
    public:
        void load();
        void unload();
        void step(double delta_time);
        void draw();
};

class Scene {
    std::vector<Actor*> actors;

    friend void start_main_loop();

    public:
        Scene();
        void add_actor(Actor *actor);
        void remove_actor(Actor *actor);
    private:
        void load();
        void unload();
        void step(double delta_time);
        void draw();
};

/*
 * render.cpp
 */

GLFWwindow *make_window(int width, int height, std::string title);
void set_scene();
void start_main_loop();
void add_key_callback(GLFWkeyfun f);
int check_gl_error();
void print_gl_log(GLuint, PFNGLGETSHADERIVPROC, PFNGLGETSHADERINFOLOGPROC);

/*
 * io.cpp
 */

namespace io {
    std::string read_file(std::string filename);
}

/*
 * util.cpp
 */

namespace util {
    lwgl::Mesh make_cube();
    void quit_on_escape_keypress(GLFWwindow*, int, int, int, int);
}

}

#endif
