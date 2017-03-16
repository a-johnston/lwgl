#ifndef lwgl_shader_cpp

#define lwgl_shader_cpp

#include "lwgl.hpp"

// Actor base class

void lwgl::Actor::load() {}

void lwgl::Actor::unload() {}

void lwgl::Actor::step(double delta_time) {
    (void) delta_time;
}

void lwgl::Actor::draw() {}

// Scene

lwgl::Scene::Scene() {

}

void lwgl::Scene::load() {

}

#endif
