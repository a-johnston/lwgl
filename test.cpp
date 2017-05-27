#include <lwgl.hpp>
#include <SOIL/SOIL.h>

class TestActor : public lwgl::Actor {
    lwgl::Mesh mesh = lwgl::util::make_cube();
    lwgl::Shader shader = lwgl::Shader("assets/color_vertex.glsl", "assets/color_fragment.glsl");
    GLint texture;

    glm::quat rotation = glm::quat(glm::vec3(1.0f, 2.0f, 0.0f));
    glm::quat orientation;

    lwgl::Camera camera;

    public:
        TestActor() {
            this->texture = SOIL_load_OGL_texture(
                "/home/adam/Pictures/hendog.png",
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT
            );

            this->shader.map_attribute(lwgl::POSITION, "position");
            this->shader.map_attribute(lwgl::NORMAL, "normal");
            this->shader.map_attribute(lwgl::TEXCOORD, "uv");

            this->shader.map_uniform("mvp", lwgl::MATRIX_4FV);
            this->shader.map_uniform("model", lwgl::MATRIX_4FV);

            this->shader.map_uniform("texture", lwgl::TEXTURE_2D, 0);
            this->shader.set_uniform("texture", this->texture);

            this->mesh.buffer();
        }

        void load() {}

        void unload() {}

        void step(const float delta_time) {
            orientation *= glm::mix(glm::quat(), rotation, delta_time);
        }

        void draw() {
            this->shader.set_uniform("mvp", this->camera.mvp(glm::toMat4(this->orientation)));
            this->shader.set_uniform("model", glm::toMat4(this->orientation));
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
