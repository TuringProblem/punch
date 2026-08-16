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

inline auto operator<<(std::ostream &os, const Player &player)
    -> std::ostream & {
  os << "Player Name: " << player.name << "\n";
  os << "Hand: " << player.hand << "\n";
  os << "Health: " << player.health << "\n";
  os << "Has Blocks: " << player.hasBlocks << "\n";
  os << "Blocks: " << player.blocks << "\n";
  return os;
}

#endif // PLAYER_HPP
