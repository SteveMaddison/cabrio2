//
// Class for loading images form files.
//
#include "Image.hpp"
#include "Logger.hpp"

Logger& logger = Logger::get_instance();

Image::Image() {
  logger.log(LOG_DEBUG, "New Image()");
  image_data = NULL;
}

Image::Image(std::string path) {
  logger.log(LOG_DEBUG, "New Image(" + path + ")");
  image_data = NULL;

  logger.log(LOG_DEBUG, "Loading image from " + path);
  image_data = IMG_Load(path.c_str());

  if (image_data == NULL) {
    logger.log(LOG_ERROR, "Error loading image from " + path);
  }
}

Image::~Image() {
  if (image_data) {
    SDL_FreeSurface(image_data);
  }
  image_data = NULL;
}

SDL_Surface *Image::surface() {
  return image_data;
}

int Image::width() {
  if (image_data) {
    return image_data->w;
  }
  return 0;
}

int Image::height() {
  if (image_data) {
    return image_data->h;
  }
  return 0;
}
