#ifndef AGENT_HPP
#define AGENT_HPP

#include "betting.hpp"
#include "bot.hpp"
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

// --- naive bot ---
// Reads raw hole card ranks and nothing else. Kept as a baseline so the
// smart bots have something to be measured against.
inline Action naiveAct(const HandState &hand, const Game &game, int seatIndex) {
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

inline int naivePickTarget(const HandState &hand, const Game &game,
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

inline GuardChoice naiveGuard(const HandState &hand, const Game &game,
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


inline Agent makeNaiveBot() {
  return Agent{naiveAct, naivePickTarget, naiveGuard};
}

// A few distinct personalities so one read does not cover the whole table.
inline const std::array<BotProfile, MAX_SEATS - 1> BOT_PROFILES = {{
    {0.62, 0.22, 0.20, 0.04, 0.05, 120},   // Southpaw: busy, bluffs a lot
    {0.45, 0.10, 0.38, -0.06, -0.05, 120}, // Ivan: quiet, traps hard
    {0.70, 0.16, 0.15, 0.00, 0.10, 120},   // Mac: straightforward pressure
    {0.50, 0.28, 0.30, 0.08, 0.00, 120},   // Duke: loose and slippery
    {0.58, 0.14, 0.25, -0.02, 0.08, 120},  // Rocco: balanced
}};

inline Agent makeBot(const BotProfile &profile) {
  return Agent{
      [profile](const HandState &hand, const Game &game, int seatIndex) {
        return smartAct(profile, hand, game, seatIndex);
      },
      [profile](const HandState &hand, const Game &game, int seatIndex) {
        return smartPickTarget(profile, hand, game, seatIndex);
      },
      [profile](const HandState &hand, const Game &game, int seatIndex,
                int attacker) {
        return smartGuard(profile, hand, game, seatIndex, attacker);
      },
  };
}

inline Agent makeBot(int seatIndex = 0) {
  const std::size_t count = BOT_PROFILES.size();
  return makeBot(BOT_PROFILES[(static_cast<std::size_t>(seatIndex) + count - 1) %
                              count]);
}

#endif
