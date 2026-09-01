#ifndef SEAT_HPP
#define SEAT_HPP

#include "cards.hpp"
#include "player.hpp"
#include <array>

/**
 * @author: { @Override } : 20260831
 * -----
 *  A Seat is PER HAND state. It is built fresh every deal and thrown away
 *  when the hand ends. Player holds what survives (health, blocks).
 *
 *  committed is the whole damage model: it is the HP this player has put
 *  into the punch pot, and therefore exactly what they take if a punch
 *  lands on them and they do not block. Fold early, commit little, take
 *  little. Ride a big raise and you are on the hook for all of it.
 **/

enum class SeatStatus { IN, FOLDED, ALL_IN };

struct Seat {
  PlayerId id = 0;
  std::array<Card, 2> hole{};
  int committed = 0;
  SeatStatus status = SeatStatus::IN;
  int streetCommitted = 0;
  bool hasActed = false;

  bool hasFolded() const noexcept { return status == SeatStatus::FOLDED; }
  bool inHand() const noexcept { return status != SeatStatus::FOLDED; }
  bool canAct() const noexcept { return status == SeatStatus::IN; }
};

#endif
