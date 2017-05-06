#ifndef lwgl_mesh_cpp

#define lwgl_mesh_cpp

#include "lwgl.hpp"

lwgl::Mesh lwgl::util::make_cube() {
    lwgl::Mesh cube = lwgl::Mesh(2, lwgl::POSITION, lwgl::NORMAL);
/*
    glm::vec4 p[] = {
        glm::vec4(-1, -1, -1, 1),
        glm::vec4( 1, -1, -1, 1),
        glm::vec4(-1,  1, -1, 1),
        glm::vec4( 1,  1, -1, 1),
        glm::vec4(-1, -1,  1, 1),
        glm::vec4( 1, -1,  1, 1),
        glm::vec4(-1,  1,  1, 1),
        glm::vec4( 1,  1,  1, 1)
    };
*/


    return cube;
}

void lwgl::util::quit_on_escape_keypress(GLFWwindow *window, int key, int scancode, int action, int mods) {
    (void) scancode;
    (void) mods;

    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

#endif
