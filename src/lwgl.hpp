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

#define LWGL_MAX_VERTEX_ATTR 32
typedef int LWGL_VERTEX_ATTR;
typedef int LWGL_VERTEX_ATTR_SIZE;

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
    std::map<LWGL_VERTEX_ATTR, GLuint> attr_handles;
    std::vector<shader_uniform> uniforms;

    friend class Mesh;

    public:
        Shader(std::string vertex_filename, std::string fragment_filename);
        ~Shader();
        int map_attribute(LWGL_VERTEX_ATTR mesh_attr, std::string shader_attr);
        shader_uniform *map_uniform(ShaderUniformType type, std::string handle, int count);
};

/*
 * mesh.cpp
 */

enum VertexAttr {
    POSITION = 0,
    NORMAL,
    BINORMAL,
    TEXCOORD,
    COLOR
};

class Mesh {
    // maxnum restricted by GL_MAX_VERTEX_ATTRIBS
    static LWGL_VERTEX_ATTR ATTR_SIZES[LWGL_MAX_VERTEX_ATTR];

    GLuint vert_vbo;
    GLuint tri_vbo;

    std::vector<LWGL_VERTEX_ATTR> format;
    int vertex_spec_size;

    std::vector<float> verts;
    std::vector<glm::ivec3> tris;

    public:
        Mesh(int num_attrs, ...);
        int add_vertex(int num_attrs, ...);
        int add_tri(glm::ivec3);
        int add_tri(int, int, int);
        void buffer();
        void unbuffer();
        void draw(Shader shader);
        void __print_debug();
        static void set_attr_size(LWGL_VERTEX_ATTR, LWGL_VERTEX_ATTR_SIZE);
        static LWGL_VERTEX_ATTR_SIZE get_attr_size(LWGL_VERTEX_ATTR);
    private:
        int get_attr_off(LWGL_VERTEX_ATTR attr);
        int get_attr_id(LWGL_VERTEX_ATTR attr, int vid);
        void set_attr(LWGL_VERTEX_ATTR attr, int vid, ...);
        void bind_to_shader(Shader shader);
        void draw_mesh_tris();
        void unbind_from_shader(Shader shader);
};

/*
 * game.cpp
 */

class Actor {
    public:
        virtual void load() = 0;
        virtual void unload() = 0;
        virtual void step(double delta_time) = 0;
        virtual void draw() = 0;
};

class Scene {
    std::vector<Actor*> actors;

    friend void start_main_loop();
    friend void set_scene(Scene*);

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
void set_scene(Scene *scene);
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
