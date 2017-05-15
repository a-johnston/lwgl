#ifndef lwgl_mesh_cpp

#define lwgl_mesh_cpp

#include "lwgl.hpp"

// maxnum restricted by GL_MAX_VERTEX_ATTRIBS
static LWGL_VERTEX_ATTR ATTR_SIZES[LWGL_MAX_VERTEX_ATTR];

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

lwgl::Mesh::Mesh(std::vector<LWGL_VERTEX_ATTR> f) {
    this->vertex_spec_size = 0;

    for (LWGL_VERTEX_ATTR attr : f) {
        this->attr_offset[attr] = this->vertex_spec_size;
        this->vertex_spec_size += lwgl::Mesh::get_attr_size(attr);
    }

    this->vert_vbo = -1;
    this->tri_vbo = -1;

    this->verts = std::vector<float>();
    this->tris = std::vector<glm::ivec3>();

    this->format = f;
}

void lwgl::Mesh::set_attr_size(LWGL_VERTEX_ATTR attr, LWGL_VERTEX_ATTR_SIZE size) {
    if (ATTR_SIZES[attr] == 0) {
        if (size > 0 && size < 5) {
            ATTR_SIZES[attr] = size;
        } else {
            std::cerr << "Illegal size value " << size << " must be in [1, 2, 3, 4].\n";
        }
    } else {
        std::cerr << "Can't redefine attribute size for " << attr << '\n';
    }
}

LWGL_VERTEX_ATTR_SIZE lwgl::Mesh::get_attr_size(LWGL_VERTEX_ATTR attr) {
    if (ATTR_SIZES[attr] == 0) {
        std::cerr << "Reading unset attribute size for attribute " << attr << '\n';
    }
    return ATTR_SIZES[attr];
}

int lwgl::Mesh::get_num_verts() {
    return this->verts.size() / this->vertex_spec_size;
}

int lwgl::Mesh::get_num_tris() {
    return this->tris.size();
}

int lwgl::Mesh::add_vertex(std::vector<LWGL_VERTEX_ATTR> format, ...) {
    int vid = this->get_num_verts();
    this->verts.resize(this->verts.size() + this->vertex_spec_size, 0.0f);

    va_list ap;
    va_start(ap, format);
    this->set_vertex(vid, format, ap);
    va_end(ap);

    return vid;
}

void lwgl::Mesh::set_vertex(int vid, std::vector<LWGL_VERTEX_ATTR> format, ...) {
    va_list ap;
    va_start(ap, format);
    this->set_vertex(vid, format, ap);
    va_end(ap);
}

void lwgl::Mesh::set_vertex(int vid, std::vector<LWGL_VERTEX_ATTR> format, va_list ap) {
    for (LWGL_VERTEX_ATTR attr : format) {
        this->set_vertex_attr(vid, attr, ap);
    }
}

void lwgl::Mesh::set_vertex_attr(int vid, LWGL_VERTEX_ATTR attr, ...) {
    va_list ap;
    va_start(ap, attr);
    this->set_vertex_attr(vid, attr, ap);
    va_end(ap);
}

void lwgl::Mesh::set_vertex_attr(int vid, LWGL_VERTEX_ATTR attr, va_list ap) {
    float *vertex = (float*) this->get_vertex(vid, attr);

    if (!vertex) {
        return;
    }

    int size = lwgl::Mesh::get_attr_size(attr);
    glm::vec4 vec = unload_vec(size, ap);

    for (int i = 0; i < size; i++) {
        vertex[i] = vec[i];
    }
}

void *lwgl::Mesh::get_vertex(int vid, LWGL_VERTEX_ATTR attr) {
    if (this->attr_offset.count(attr) == 0) {
        return NULL;
    }

    return &this->verts[vid * this->vertex_spec_size + this->attr_offset[attr]];
}

int lwgl::Mesh::add_tri(glm::ivec3 tri) {
    this->tris.push_back(tri);
    return this->tris.size() - 1;
}

int lwgl::Mesh::add_tri(int i, int j, int k) {
    return this->add_tri(glm::ivec3(i, j, k));
}

int lwgl::Mesh::add_quad(glm::ivec4 quad) {
    return this->add_quad(quad.x, quad.y, quad.z, quad.w);
}

int lwgl::Mesh::add_quad(int i, int j, int k, int l) {
    this->add_tri(i, j, k);
    return this->add_tri(l, i, k);
}

int lwgl::Mesh::build_tri(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    glm::vec3 norm = lwgl::util::get_normal(a, b, c);
    std::vector<LWGL_VERTEX_ATTR> format = { POSITION, NORMAL };

    return this->add_tri({
        this->add_vertex(format, a, norm),
        this->add_vertex(format, b, norm),
        this->add_vertex(format, c, norm)
    });
}

int lwgl::Mesh::build_tri(glm::ivec3 tri, glm::vec3 *p) {
    return this->build_tri(p[tri.x], p[tri.y], p[tri.z]);
}

int lwgl::Mesh::build_quad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d) {
    glm::vec3 norm = lwgl::util::get_normal(a, b, c, d);
    std::vector<LWGL_VERTEX_ATTR> format = { POSITION, NORMAL, TEXCOORD };

    return this->add_quad({
        this->add_vertex(format, a, norm, glm::vec2(0, 0)),
        this->add_vertex(format, b, norm, glm::vec2(0, 1)),
        this->add_vertex(format, c, norm, glm::vec2(1, 1)),
        this->add_vertex(format, d, norm, glm::vec2(1, 0))
    });
}

int lwgl::Mesh::build_quad(glm::ivec4 quad, glm::vec3 *p) {
    return this->build_quad(p[quad.x], p[quad.y], p[quad.z], p[quad.w]);
}

static GLuint make_buffer(const void *buffer_data, GLsizei buffer_size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, buffer_data, GL_STATIC_DRAW);
    return buffer;
}

// TODO: this rewrite actually could pretty easily support homogeneous
// position coordinates, which I'm sure would be useful to people who actually
// knew how to do interesting things, but hardcoding attributes such as in the
// following methods makes these methods unnecessarily incompatible with using
// homogeneous coordinates, ironically. Potentially, there could be overloaded
// versions which accepted glm::vec4, but that isn't a very intuitive solution.

void lwgl::Mesh::translate(glm::vec3 vec) {
    for (int i = 0; i < this->get_num_verts(); i++) {
        *((glm::vec3*) this->get_vertex(i, lwgl::POSITION)) += vec;
    }
}

void lwgl::Mesh::scale(glm::vec3 vec) {
    for (int i = 0; i < this->get_num_verts(); i++) {
        *((glm::vec3*) this->get_vertex(i, lwgl::POSITION)) *= vec;
    }
}

void lwgl::Mesh::scale(float m) {
    for (int i = 0; i < this->get_num_verts(); i++) {
        *((glm::vec3*) this->get_vertex(i, lwgl::POSITION)) *= m;
    }
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
    std::cout << "vss\t\t: " << this->vertex_spec_size << '\n';

    printf("Vertexes\n\n  #  ");
    char buffer[52];
    for (LWGL_VERTEX_ATTR attr : this->format) {
        int width = 13 * lwgl::Mesh::get_attr_size(attr) - 1;
        sprintf(buffer, "%%%dd%%20s", width - 20);
        printf(buffer, attr, "");
    }
    std::cout << '\n';

    int c = 0;
    for (float val : this->verts) {
        if (c % this->vertex_spec_size == 0) {
            printf("%-5d ", c / this->vertex_spec_size);
        }
        printf("%10f", val);
        c += 1;
        if (c % this->vertex_spec_size == 0) {
            std::cout << '\n';
        } else {
            std::cout << " | ";
        }
    }

    printf("\n\nTriangles\n\n");
    for (glm::ivec3 tri : this->tris) {
        printf("%d %d %d\n", tri.x, tri.y, tri.z);
    }
}

void lwgl::Mesh::bind_to_shader(Shader shader) {
    glUseProgram(shader.prog);
    glBindBuffer(GL_ARRAY_BUFFER, this->vert_vbo);
    for (LWGL_VERTEX_ATTR a : this->format) {
        if (shader.attr_handles.count(a)) {
            glVertexAttribPointer(
                shader.attr_handles[a],
                lwgl::Mesh::get_attr_size(a),
                GL_FLOAT,
                GL_FALSE,
                this->vertex_spec_size * sizeof(float),
                (const GLvoid*) ((long) this->attr_offset[a] * sizeof(float))
            );
            glEnableVertexAttribArray(shader.attr_handles[a]);
        }
    }
}

void lwgl::Mesh::draw_mesh_tris() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->tri_vbo);
    glDrawElements(GL_TRIANGLES, this->tris.size() * 3, GL_UNSIGNED_INT, 0);
}

void lwgl::Mesh::unbind_from_shader(Shader shader) {
    (void) shader;
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
