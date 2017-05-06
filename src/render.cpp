#ifndef lwgl_render_cpp

#define lwgl_render_cpp

#include "lwgl.hpp"

static GLFWwindow *window = NULL;
static GLFWmonitor *monitor = NULL;
static lwgl::Scene *scene = NULL;

static int _width, _height;

static std::vector<GLFWkeyfun> key_callbacks;

static void resize_callback(GLFWwindow *window, int width, int height) {
    (void) window;
    _width = width;
    _height = height;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    for (GLFWkeyfun f : key_callbacks) {
        (*f)(window, key, scancode, action, mods);
    }
}

void lwgl::print_gl_log(
    GLuint object,
    PFNGLGETSHADERIVPROC glGet__iv,
    PFNGLGETSHADERINFOLOGPROC glGet__InfoLog
) {
    GLint log_length;
    glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);

    char *log = (char*) malloc(log_length);
    glGet__InfoLog(object, log_length, NULL, log);
    std::cerr << log << '\n';
    free(log);
}

int lwgl::check_gl_error() {
    int err = glGetError();

    if (err) {
        std::cerr << "OpenGL Error " << err << '\n';
    }

    return err;
}

void lwgl::add_key_callback(GLFWkeyfun f) {
    key_callbacks.push_back(f);
}

void lwgl::set_scene(Scene *s) {
    if (scene) {
        scene->unload();
    }
    if (s) {
        s->load();
    }
    scene = s;
}

GLFWwindow *lwgl::make_window(int width, int height, std::string title) {
    Mesh::set_attr_size(lwgl::POSITION, 3);
    Mesh::set_attr_size(lwgl::NORMAL, 3);
    Mesh::set_attr_size(lwgl::BINORMAL, 3);
    Mesh::set_attr_size(lwgl::TEXCOORD, 2);
    Mesh::set_attr_size(lwgl::COLOR, 4);

    if (window) {
        return window;
    }

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!\n";
        exit(EXIT_FAILURE);
    }

    if (width < 1 && height < 1) {
        monitor = glfwGetPrimaryMonitor();

        if (!monitor) {
            std::cerr << "Failed to get primary monitor!\n";
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        width = mode->width;
        height = mode->height;
    }

    glfwWindowHint(GLFW_SAMPLES, 1);

    window = glfwCreateWindow(width, height, title.c_str(), monitor, NULL);

    _width = width;
    _height = height;

    if (!window) {
        std::cerr << "Failed to create window!\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    if (monitor) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resize_callback);
    glfwSetKeyCallback(window, key_callback);

    GLenum err;
    if ((err = glewInit()) != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << '\n';
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    if (!GLEW_VERSION_3_0) {
        std::cerr << "OpenGL 3.0 not available!\n";
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    key_callbacks = std::vector<GLFWkeyfun>();

    return window;
}

void lwgl::start_main_loop() {
    glfwSetTime(0.0);
    double temp, time = 0.0;

    while (!glfwWindowShouldClose(window)) {
        lwgl::check_gl_error();
        glfwPollEvents();

        temp = glfwGetTime();
        if (scene) {
            scene->step(temp - time);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            scene->draw();
            glfwSwapBuffers(window);
        }
        time = temp;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

#endif
