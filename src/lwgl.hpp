#ifndef lwgl_hpp

#define lwgl_hpp

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#define LWGL_MAX_VERTEX_ATTR 32
#define check_gl_error() lwgl::__check_gl_error(__FILE__,__LINE__)
#define wrap_gl_error(CODE) if (1) { CODE; lwgl::__check_gl_error(__FILE__,__LINE__, #CODE); }
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

typedef struct {
    ShaderUniformType type;
    GLint handle;
    int count;
} shader_uniform;

class Shader {
    GLuint vert, frag, prog;
    std::unordered_map<LWGL_VERTEX_ATTR, GLuint> attr_handles;
    std::unordered_map<std::string, shader_uniform> uniforms;

    friend class Mesh;

    public:
        Shader(std::string vertex_filename, std::string fragment_filename);
        ~Shader();
        int map_attribute(LWGL_VERTEX_ATTR mesh_attr, std::string shader_attr);
        int map_uniform(std::string, ShaderUniformType, int=1);
        int set_uniform(std::string, ...);
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
    GLuint vert_vbo;
    GLuint tri_vbo;

    std::vector<float> verts;
    std::vector<glm::ivec3> tris;

    public:
        std::vector<LWGL_VERTEX_ATTR> format;
        std::unordered_map<LWGL_VERTEX_ATTR, int> attr_offset;
        int vertex_spec_size;

        Mesh(std::vector<LWGL_VERTEX_ATTR>);

        int get_num_verts();
        int get_num_tris();

        int add_vertex(std::vector<LWGL_VERTEX_ATTR>, ...);
        void set_vertex(int, std::vector<LWGL_VERTEX_ATTR>, va_list);
        void set_vertex(int, std::vector<LWGL_VERTEX_ATTR>, ...);
        void set_vertex_attr(int, LWGL_VERTEX_ATTR, va_list);
        void set_vertex_attr(int, LWGL_VERTEX_ATTR, ...);
        void *get_vertex(int, LWGL_VERTEX_ATTR);

        int add_tri(glm::ivec3);
        int add_tri(int, int, int);
        int add_quad(glm::ivec4);
        int add_quad(int, int, int, int);

        int build_tri(glm::vec3, glm::vec3, glm::vec3);
        int build_tri(glm::ivec3, glm::vec3*);
        int build_quad(glm::vec3, glm::vec3, glm::vec3, glm::vec3);
        int build_quad(glm::ivec4, glm::vec3*);

        void translate(glm::vec3);
        void scale(glm::vec3);
        void scale(float);

        void buffer();
        void unbuffer();
        void draw(Shader shader);

        void __print_debug();

        static void set_attr_size(LWGL_VERTEX_ATTR, LWGL_VERTEX_ATTR_SIZE);
        static LWGL_VERTEX_ATTR_SIZE get_attr_size(LWGL_VERTEX_ATTR);
    private:
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

class Camera {
    glm::mat4 view;
    glm::mat4 pers;
    glm::mat4 vp;

    public:
        glm::vec3 from, to, up;
        float fov, aspect, znear, zfar;

        Camera(
            glm::vec3 = glm::vec3(2, 2, 2),
            glm::vec3 = glm::vec3(0, 0, 0),
            glm::vec3 = glm::vec3(0, 0, 1),
            float = 90.0f,
            float = 0.0f,
            float = 0.1f,
            float = 100.0f);

        void update_view();
        void update_pers();
        glm::mat4 mvp(glm::mat4 model);
};

/*
 * render.cpp
 */

GLFWwindow *make_window(int width, int height, std::string title);
void set_scene(Scene *scene);
void start_main_loop();
void add_key_callback(GLFWkeyfun f);
void __check_gl_error(const char *file, int line);
void __check_gl_error(const char *file, int line, const char *code);
void print_gl_log(GLuint, PFNGLGETSHADERIVPROC, PFNGLGETSHADERINFOLOGPROC);
float get_aspect_ratio();

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
    lwgl::Mesh make_plane();
    lwgl::Mesh make_subdivided_plane(int n);
    void quit_on_escape_keypress(GLFWwindow*, int, int, int, int);

    glm::vec3 get_normal(glm::vec3, glm::vec3, glm::vec3);
    glm::vec3 get_normal(glm::vec3, glm::vec3, glm::vec3, glm::vec3);
    glm::vec3 get_normal(glm::ivec3 idx, glm::vec3 *v);
    glm::vec3 get_normal(glm::ivec4 idx, glm::vec3 *v);
}

}

#endif
