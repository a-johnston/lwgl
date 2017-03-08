#include <lwgl.hpp>

int main() {
    lwgl::Mesh mesh = lwgl::Mesh("vnc");

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
