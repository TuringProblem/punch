#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "cardpair.hpp"
#include "cards.hpp"
#include <string>

/**
 * @author: { @Override } : 20260816 13:47
 * -----
 *  see the docs/IMPLEMENTATION_DESIGN.md for more details
 **/

struct Player {
  std::string name;
  CardPair<CardType, SuitType> hand;
  int health;
  bool hasBlocks;
  int blocks;
};

#endif // PLAYER_HPP
