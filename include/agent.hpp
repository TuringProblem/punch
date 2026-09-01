#ifndef AGENT_HPP
#define AGENT_HPP

#include "betting.hpp"
#include "punch.hpp"
#include <functional>

/**
 * @author: { @Override } : 20260831
 *
 * How a seat decides what to do. Three callables, no base class, so a human
 * and a bot are the same type. The same seam swaps in a network peer later
 * for multiplayer.
 **/

struct Agent {
  std::function<Action(const HandState &, const Game &, int)> act;
  std::function<int(const HandState &, const Game &, int)> pickTarget;
  std::function<GuardChoice(const HandState &, const Game &, int, int)> guard;
};

inline Action botAct(const HandState &hand, const Game &game, int seatIndex) {
  const Seat &seat = hand.seats[static_cast<std::size_t>(seatIndex)];
  const int owed = toCall(hand, seatIndex);

  const int a = static_cast<int>(seat.hole[0].type);
  const int b = static_cast<int>(seat.hole[1].type);
  const bool pair = a == b;
  const bool ace =
      seat.hole[0].type == CardType::ACE || seat.hole[1].type == CardType::ACE;
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

inline int botPickTarget(const HandState &hand, const Game &game,
                         int seatIndex) {
  const std::vector<int> targets = targetsFor(hand, game, seatIndex);
  if (targets.empty())
    return -1;

  std::uniform_int_distribution<int> roll(0, 99);
  if (roll(rng()) < 25) {
    std::uniform_int_distribution<std::size_t> pick(0, targets.size() - 1);
    return targets[pick(rng())];
  }

  int best = targets.front();
  for (const int seat : targets) {
    if (punchDamage(hand, seat) > punchDamage(hand, best))
      best = seat;
  }
  return best;
}

inline GuardChoice botGuard(const HandState &hand, const Game &game,
                            int seatIndex, int /*attacker*/) {
  const Player &me = game.players[static_cast<std::size_t>(
      hand.seats[static_cast<std::size_t>(seatIndex)].id)];
  if (!me.canBlock())
    return GuardChoice::NO_BLOCK;

  const int atRisk = punchDamage(hand, seatIndex);
  const int chance =
      std::clamp(15 + (atRisk * 60) / std::max(1, me.health), 15, 70);

  std::uniform_int_distribution<int> roll(0, 99);
  return roll(rng()) < chance ? GuardChoice::BLOCK : GuardChoice::NO_BLOCK;
}

inline Agent makeBot() { return Agent{botAct, botPickTarget, botGuard}; }

#endif
