#ifndef lwgl_shader_cpp

#define lwgl_shader_cpp

#include "lwgl.hpp"

lwgl::Camera::Camera(
    glm::vec3 from, glm::vec3 to, glm::vec3 up,
    float fov, float aspect, float znear, float zfar
) {
    this->from = from;
    this->to = to;
    this->up = up;
    this->fov = fov;
    this->aspect = aspect;
    this->znear = znear;
    this->zfar = zfar;

    this->update_view();
    this->update_pers();
}

void lwgl::Camera::update_view() {
    this->view = glm::lookAt(from, to, up);
    this->vp = this->pers * this->view;
}

void lwgl::Camera::update_pers() {
    if (this->aspect == 0.0f) {
        this->aspect = lwgl::get_aspect_ratio();
    }

    this->pers = glm::perspective(fov, aspect, znear, zfar);
    this->vp = this->pers * this->view;
}

glm::mat4 lwgl::Camera::mvp(glm::mat4 model) {
    return this->vp * model;
}

lwgl::Scene::Scene() {
    this->actors = std::vector<Actor*>();
}

void lwgl::Scene::add_actor(Actor *a) {
    this->actors.push_back(a);
}

void lwgl::Scene::remove_actor(Actor *a) {
    for (auto it = this->actors.begin(); it != this->actors.end();) {
        if (*it == a) {
            this->actors.erase(it);
            return;
        }
    }
}

void lwgl::Scene::load() {
    for (Actor *a : this->actors) {
        a->load();
    }
}

void lwgl::Scene::unload() {
    for (Actor *a : this->actors) {
        a->unload();
    }
}

void lwgl::Scene::step(double delta_time) {
    for (Actor *a : this->actors) {
        a->step(delta_time);
    }
}

void lwgl::Scene::draw() {
    for (Actor *a : this->actors) {
        check_gl_error();
        a->draw();
        check_gl_error();
    }
}

#endif
