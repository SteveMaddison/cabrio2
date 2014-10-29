//
// A quad (4-sided polygon) to display on screen.
//
#ifndef CABRIO_QUAD_HPP_
#define CABRIO_QUAD_HPP_

#include <string>

#include "Image.hpp"

class Quad {
  public:
    Quad();
    Quad(std::string path);
    ~Quad();

    float x;
    float y;
    float z;
    float size_x;
    float size_y;
    float rotation_x;
    float rotation_y;
    float rotation_z;
    float alpha;

    Image *image();
    void image(Image *i);
    void image(std::string path);

    unsigned int texture_id();
    unsigned int set_texture_id(unsigned int i);
    bool texture_dirty();

  private:
    Image *image_;
    uint texture_id_;
    bool texture_dirty_;
};

#endif
