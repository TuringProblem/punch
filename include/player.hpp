#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "rules.hpp"
#include <cstdint>
#include <string>

/**
 * @author: { @Override } : 20260816 13:47
 * -----
 *  A Player is PERSISTENT state. It survives every hand until the player is
 *  knocked out. Per-hand state (hole cards, committed HP, folded or not)
 *  belongs on a Seat, not here.
 *
 *  see the docs/IMPLEMENTATION_DESIGN.md for more details
 **/

using PlayerId = std::uint8_t;

struct Player {
  PlayerId id = 0;
  std::string name;
  int health = START_HEALTH;
  int blocks = START_BLOCKS;
  bool isHuman = false;
  bool isKnockedOut = false;

  bool canBlock() const noexcept { return blocks > 0; }
};

#endif
