#ifndef AGENT_HPP
#define AGENT_HPP

#include "betting.hpp"

/**
 * @author: { @Override } : 20260831
 * -----
 *  How a seat decides what to do. One interface, so the engine never has to
 *  know whether a human or a bot is sitting there. The same seam swaps in a
 *  network peer later for multiplayer.
 **/

struct Agent {
  virtual ~Agent() = default;
  virtual Action act(const HandState &hand, const Game &game,
                     int seatIndex) = 0;
};

struct SimpleBot : Agent {
  Action act(const HandState &hand, const Game &game, int seatIndex) override {
    const Seat &seat = hand.seats[static_cast<std::size_t>(seatIndex)];
    const int owed = toCall(hand, seatIndex);

    const int a = static_cast<int>(seat.hole[0].type);
    const int b = static_cast<int>(seat.hole[1].type);
    const bool pair = a == b;
    const bool ace = seat.hole[0].type == CardType::ACE ||
                     seat.hole[1].type == CardType::ACE;
    const bool strong = pair || ace || (a >= 9 && b >= 9);

    std::uniform_int_distribution<int> roll(0, 99);
    const int r = roll(rng());

    if (owed == 0) {
      if (strong && r < 35 && canRaise(hand, game, seatIndex)) {
        return Action{ActionType::RAISE, minRaiseTo(hand, game, seatIndex)};
      }
      return Action{ActionType::CHECK, 0};
    }

    if (!strong && r < 55)
      return Action{ActionType::FOLD, 0};
    if (strong && r < 20 && canRaise(hand, game, seatIndex)) {
      return Action{ActionType::RAISE, minRaiseTo(hand, game, seatIndex)};
    }
    return Action{ActionType::CALL, 0};
  }
};

#endif
