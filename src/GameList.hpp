//
// GameList - singleton containing all games we know about.
//
#ifndef CABRIO_GAMELIST_HPP_
#define CABRIO_GAMELIST_HPP_

#include <vector>

#include "Game.hpp"
#include "Singleton.hpp"

class GameList : public Singleton<GameList> {
  friend class Singleton<GameList>;

  public:
    ~GameList();

    void add(Game& g);
    void reset();
    Game& first();
    Game& next();
    Game& previous();

  private:
    GameList();

//    GameList(GameList& const);
//    operator=(GameList& const);

    std::vector<Game> games_;
    std::vector<Game>::iterator currIter;
};

#endif
