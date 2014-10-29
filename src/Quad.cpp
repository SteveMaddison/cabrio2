//
// A quad (4-sided polygon) to display on screen.
//
#include "Quad.hpp"

Quad::Quad() {
  image_ = NULL;

  size_x = 1.0f;
  size_y = 1.0f;

  texture_id_ = 0;
  texture_dirty_ = true;
}

Quad::~Quad() {
  if ( image_ ) {
    delete image_;
  }
}

Image *Quad::image() {
  return image_;
}

void Quad::image(Image *i) {
  image_ = i;
  texture_dirty_ = true;
}

void Quad::image(std::string path) {
  Image *i = new Image(path);
  image_ = i;
  texture_dirty_ = true;
}

unsigned int Quad::texture_id() {
  return texture_id_;
}

unsigned int Quad::set_texture_id(unsigned int i) {
  texture_id_ = i;
  texture_dirty_ = false;
  return texture_id_;
}

bool Quad::texture_dirty() {
  return texture_dirty_;
}
