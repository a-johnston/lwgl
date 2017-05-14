#ifndef lwgl_mesh_cpp

#define lwgl_mesh_cpp

#include "lwgl.hpp"

lwgl::Mesh lwgl::util::make_cube() {
    lwgl::Mesh cube = lwgl::Mesh({lwgl::POSITION, lwgl::NORMAL, lwgl::TEXCOORD});

    glm::vec3 p[] = {
        glm::vec3(-1, -1, -1),
        glm::vec3( 1, -1, -1),
        glm::vec3(-1,  1, -1),
        glm::vec3( 1,  1, -1),
        glm::vec3(-1, -1,  1),
        glm::vec3( 1, -1,  1),
        glm::vec3(-1,  1,  1),
        glm::vec3( 1,  1,  1)
    };

    glm::ivec4 quads[] = {
        glm::ivec4(4, 6, 7, 5),
        glm::ivec4(1, 5, 7, 3),
        glm::ivec4(0, 2, 6, 4),
        glm::ivec4(0, 1, 3, 2),
        glm::ivec4(0, 4, 5, 1),
        glm::ivec4(2, 3, 7, 6)
    };

    for (glm::ivec4 quad : quads) {
        cube.build_quad(quad, p);
    }

    return cube;
}

lwgl::Mesh lwgl::util::make_plane() {
    lwgl::Mesh mesh = lwgl::Mesh({lwgl::POSITION, lwgl::NORMAL, lwgl::TEXCOORD});

    mesh.build_quad(
        glm::vec3(-1, -1, 0),
        glm::vec3(-1,  1, 0),
        glm::vec3( 1,  1, 0),
        glm::vec3( 1, -1, 0)
    );

    return mesh;
}

lwgl::Mesh lwgl::util::make_subdivided_plane(int n) {
    lwgl::Mesh mesh = lwgl::Mesh({lwgl::POSITION, lwgl::NORMAL});
    n += 1;

    for (int x = 0; x <= n; x++) {
        for (int y = 0; y <= n; y++) {
            mesh.add_vertex(
                mesh.format,
                glm::vec3((float) x / n, (float) y / n, 0.0f),
                glm::vec3(0, 0, 1.0f)
            );
        }
    }

    return mesh;
}

void lwgl::util::quit_on_escape_keypress(GLFWwindow *window, int key, int scancode, int action, int mods) {
    (void) scancode;
    (void) mods;

    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

glm::vec3 lwgl::util::get_normal(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    return glm::normalize(glm::cross(c - a, b - a));
}

glm::vec3 lwgl::util::get_normal(glm::ivec3 idx, glm::vec3 *p) {
    return lwgl::util::get_normal(p[idx.x], p[idx.y], p[idx.z]);
}

glm::vec3 lwgl::util::get_normal(glm::ivec4 idx, glm::vec3 *p) {
    return glm::normalize(
        lwgl::util::get_normal(p[idx.x], p[idx.y], p[idx.z]) +
        lwgl::util::get_normal(p[idx.x], p[idx.z], p[idx.w]));
}

#endif
