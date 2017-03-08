#ifndef lwgl_mesh_cpp

#define lwgl_mesh_cpp

#include "lwgl.hpp"

lwgl::Mesh::Mesh(char *format) {
    this->format = format;
    this->flength = strlen(format);

    this->vert_vbo = -1;
    this->tri_vbo = -1;

    this->verts = std::vector<glm::vec4>();
    this->tris = std::vector<glm::ivec3>();
}

int lwgl::Mesh::get_attr_off(char a) {
    char *f = this->format;

    while (*f != '\0') {
        if (*f == a) {
            return (int) (f - this->format);
        }
    }

    return -1;
}

int lwgl::Mesh::get_attr_id(char a, int vid) {
    int offset = this->get_attr_off(a);

    if (offset == -1) {
        return -1;
    }

    return vid * this->flength + offset;
}

void lwgl::Mesh::set_attr(char a, int vid, glm::vec4 value) {
    this->verts[this->get_attr_id(a, vid)] = value;
}

int lwgl::Mesh::add_vertex(char *format, ...) {
    int vid = this->verts.size() / this->flength;
    this->verts.resize(this->verts.size() + this->flength, glm::vec4());

    va_list ap;
    va_start(ap, format);

    while (*format != '\0') {
        this->set_attr(*format, vid, va_arg(ap, glm::vec4));
    }

    va_end(ap);

    return vid;
}

int lwgl::Mesh::add_tri(glm::ivec3 tri) {
    this->tris.push_back(tri);
    return this->tris.size() - 1;
}

int lwgl::Mesh::add_tri(int i, int j, int k) {
    return this->add_tri(glm::ivec3(i, j, k));
}

static GLuint make_buffer(const void *buffer_data, GLsizei buffer_size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, buffer_data, GL_STATIC_DRAW);
    return buffer;
}

static void free_buffer(GLuint buffer) {
    glDeleteBuffers(1, &buffer);
}

void lwgl::Mesh::buffer() {
    this->unbuffer();
    this->vert_vbo = make_buffer(
        this->verts.data(),
        this->verts.size() * sizeof(glm::vec4)
    );

    this->tri_vbo = make_buffer(
        this->tris.data(),
        this->tris.size() * sizeof(glm::ivec3)
    );
}

void lwgl::Mesh::unbuffer() {
    if (this->vert_vbo > 0) {
        free_buffer(this->vert_vbo);
    }

    if (this->tri_vbo > 0) {
        free_buffer(this->tri_vbo);
    }
}

#endif
