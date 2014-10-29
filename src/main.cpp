//
// One day, there will be some code here.
//
#include <iostream>
#include <unistd.h>
using namespace std;

#include "GameList.hpp"
#include "Quad.hpp"
#include "Logger.hpp"
#include "RendererOpenGL.hpp"

int main(int argc, char* argv[]) {
  GameList* list = GameList::get_instance();
  Logger& logger = Logger::get_instance();
  Renderer* renderer = new RendererOpenGL();

  logger.level(LOG_DEBUG);
  logger.log(LOG_INFO, "Starting up");

  Game g1("Hello");
  Game g2("Goodbye");
  Quad q1;
  q1.image("./share/images/cabrio_logo.png");
  q1.size_x = 3;
  q1.size_y = 1;

  renderer->init();
  renderer->quads.push_back(q1);

  list->add(g1);
  list->add(g2);

  cout << list->first().name() << endl;
  cout << list->next().name() << endl;
  cout << list->next().name() << endl;

  list->reset();
  cout << list->first().name() << endl;
  cout << list->previous().name() << endl;
  cout << list->previous().name() << endl;

  float offset = 0.01f;

  while(true) {
    renderer->quads.at(0).x += offset * 10.0f;
    renderer->quads.at(0).y += offset * 4.0f;
    renderer->quads.at(0).z += offset;

    if ( renderer->quads.at(0).x > 5.0 || renderer->quads.at(0).x < -5.0 ) {
      offset = -offset;
    }

    renderer->quads.at(0).rotation_x += 1.0f;
    renderer->quads.at(0).rotation_y += 1.0f;
    renderer->quads.at(0).rotation_z += 1.0f;

    renderer->draw_all();
    usleep(40000);
  }

  return 0;
}
