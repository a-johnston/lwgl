#ifndef lwgl_shader_cpp

#define lwgl_shader_cpp

#include "lwgl.hpp"

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
        a->draw();
    }
}

#endif
