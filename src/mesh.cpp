#ifndef lwgl_mesh_cpp

#define lwgl_mesh_cpp

#include "lwgl.hpp"

static glm::vec4 unload_vec(int n, va_list ap) {
    if (n == 2) {
        return glm::vec4(va_arg(ap, glm::vec2), 0, 0);
    } else if (n == 3) {
        return glm::vec4(va_arg(ap, glm::vec3), 0);
    } else if (n == 4) {
        return va_arg(ap, glm::vec4);
    }
    return glm::vec4(0);
}

lwgl::Mesh::Mesh(int num_attrs, ...) {
    va_list ap;
    va_start(ap, num_attrs);

    for (int i = 0; i < num_attrs; i++) {
        LWGL_VERTEX_ATTR attr = va_arg(ap, LWGL_VERTEX_ATTR);
        this->format.push_back(attr);
        this->vertex_spec_size += lwgl::Mesh::get_attr_size(attr);
    }

    va_end(ap);

    this->vert_vbo = -1;
    this->tri_vbo = -1;

    this->verts = std::vector<float>();
    this->tris = std::vector<glm::ivec3>();
}

void lwgl::Mesh::set_attr_size(LWGL_VERTEX_ATTR attr, LWGL_VERTEX_ATTR_SIZE size) {
    if (lwgl::Mesh::ATTR_SIZES[attr] == 0) {
        if (size > 0 && size < 5) {
            lwgl::Mesh::ATTR_SIZES[attr] = size;
        } else {
            std::cerr << "Illegal size value " << size << " must be in [1, 2, 3, 4].\n";
        }
    } else {
        std::cerr << "Can't redefine attribute size for " << attr << '\n';
    }
}

LWGL_VERTEX_ATTR_SIZE lwgl::Mesh::get_attr_size(LWGL_VERTEX_ATTR attr) {
    if (lwgl::Mesh::ATTR_SIZES[attr] == 0) {
        std::cerr << "Reading unset attribute size for attribute " << attr << '\n';
    }
    return lwgl::Mesh::ATTR_SIZES[attr];
}

/*
int lwgl::Mesh::get_attr_off(LWGL_VERTEX_ATTR attr) {
    int off = 0;

    for (LWGL_VERTEX_ATTR a : this->format) {
        if (a == attr) {
            return off;
        }

        off += lwgl::Mesh::get_attr_size(a);
    }

    return -1;
}

int lwgl::Mesh::get_attr_id(LWGL_VERTEX_ATTR attr, int vid) {
    int offset = this->get_attr_off(attr);

    if (offset == -1) {
        return -1;
    }

    return vid * this->vertex_spec_size + offset;
}

void lwgl::Mesh::set_attr(LWGL_VERTEX_ATTR attr, int vid, ...) {
    int idx = this->get_attr_id(attr, vid);

    va_list ap;
    va_start(ap, vid);

    for (int i = 0; i < lwgl::Mesh::get_attr_size(attr); i++) {
        this->verts[idx + i] = va_arg(ap, float);
    }

    va_end(ap);
}
*/

int lwgl::Mesh::add_vertex(int num_attrs, ...) {
    int idx = this->verts.size();
    this->verts.resize(this->verts.size() + this->vertex_spec_size, 0.0f);

    va_list ap;
    va_start(ap, num_attrs);

    glm::vec4 vec;
    for (LWGL_VERTEX_ATTR a : format) {
        int size = lwgl::Mesh::get_attr_size(a);
        vec = unload_vec(size, ap);

        for (int i = 0; i < size; i++) {
            this->verts[idx + i] = vec[i];
        }
    }

    va_end(ap);

    return idx / this->vertex_spec_size;
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
        this->verts.size() * sizeof(float)
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

void lwgl::Mesh::__print_debug() {
    std::cout << "verts size\t: " << this->verts.size() << '\n';
    std::cout << "tris size\t: " << this->tris.size() << '\n';
    std::cout << "num verts\t: " << (this->verts.size() / this->vertex_spec_size) << '\n';

    int c = 0;
    for (float val : this->verts) {
        std::cout << val;
        c += 1;
        if (c % this->vertex_spec_size == 0) {
            std::cout << '\n';
        } else {
            std::cout << " | ";
        }
    }
}

void lwgl::Mesh::bind_to_shader(Shader shader) {
    int stride = this->vertex_spec_size;
    long pointer = 0;

    glBindBuffer(GL_ARRAY_BUFFER, this->vert_vbo);
    for (LWGL_VERTEX_ATTR a : this->format) {
        if (shader.attr_handles.count(a)) {
            glVertexAttribPointer(
                shader.attr_handles[a],
                4,
                GL_FLOAT,
                GL_FALSE,
                stride,
                (const GLvoid*) pointer
            );
            glEnableVertexAttribArray(shader.attr_handles[a]);
        }
        pointer += lwgl::Mesh::get_attr_size(a);
    }
}

void lwgl::Mesh::draw_mesh_tris() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->tri_vbo);
    glDrawElements(GL_TRIANGLES, this->tris.size() * 3, GL_UNSIGNED_INT, 0);
}

void lwgl::Mesh::unbind_from_shader(Shader shader) {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    for (LWGL_VERTEX_ATTR a : this->format) {
        if (shader.attr_handles.count(a)) {
            glDisableVertexAttribArray(shader.attr_handles[a]);
        }
    }
}

void lwgl::Mesh::draw(Shader shader) {
    this->bind_to_shader(shader);
    this->draw_mesh_tris();
    this->unbind_from_shader(shader);
}

#endif
