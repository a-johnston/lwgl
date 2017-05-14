#include <lwgl.hpp>

class TestActor : public lwgl::Actor {
    lwgl::Mesh mesh = lwgl::util::make_cube();
    lwgl::Shader shader = lwgl::Shader("assets/color_vertex.glsl", "assets/color_fragment.glsl");

    public:
        TestActor() {
            lwgl::Camera camera;

            this->shader.map_attribute(lwgl::POSITION, "position");
            this->shader.map_attribute(lwgl::NORMAL, "normal");

            this->shader.map_uniform("mvp", lwgl::MATRIX_4FV);
            this->shader.set_uniform("mvp", camera.mvp(glm::mat4(1)));

            this->mesh.buffer();

            this->mesh.__print_debug();
        }

        void load() {}

        void unload() {}

        void step(double delta_time) {
            (void) delta_time;
        }

        void draw() {
            wrap_gl_error(this->mesh.draw(this->shader));
        }
};

int main() {
    lwgl::make_window(640, 480, "test");
    lwgl::add_key_callback(&lwgl::util::quit_on_escape_keypress);

    TestActor test_actor;

    lwgl::Scene scene;
    scene.add_actor(&test_actor);

    lwgl::set_scene(&scene);
    lwgl::start_main_loop();

    return 0;
}
