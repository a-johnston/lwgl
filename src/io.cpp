#ifndef lwgl_io_cpp

#define lwgl_io_cpp

#include "lwgl.hpp"

std::string lwgl::io::read_file(std::string filename) {
    std::ifstream t = std::ifstream(filename);
    std::stringstream buf;
    buf << t.rdbuf();
    return buf.str();
}

#endif
