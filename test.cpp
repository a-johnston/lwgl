#include <lwgl.hpp>

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    (void) scancode;
    (void) mods;
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main() {
    lwgl::make_window(640, 480, "test");
    lwgl::add_key_callback(&key_callback);

    lwgl::Mesh mesh = lwgl::Mesh("vnc");
    lwgl::Shader shader = lwgl::Shader("assets/color_vertex.glsl", "assets/color_fragment.glsl");

    shader.map_attribute('v', "position");
    shader.map_attribute('n', "normal");

    shader.map_uniform(lwgl::MATRIX_4FV, "mvp", 1);
    shader.map_uniform(lwgl::MATRIX_4FV, "model", 1);

    glm::vec4 v = glm::vec4(1, 0, 0, 0);
    glm::vec4 n = glm::vec4(0, 1, 0, 0);
    glm::vec4 c = glm::vec4(0, 0, 1, 0);
    int i = mesh.add_vertex("cvn", c, v, n);
    int j = mesh.add_vertex("vnc", v, n, c);
    int k = mesh.add_vertex("ncv", n, c, v);

    mesh.add_tri(i, j, k);

    mesh.__print_debug();

    return 0;
}
