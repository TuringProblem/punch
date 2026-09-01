#ifndef BOT_HPP
#define BOT_HPP

#include "betting.hpp"
#include "equity.hpp"
#include "punch.hpp"
#include <string>

/**
 * @author: { @Override } : 20260831
 *
 * Bots that actually read the board, without wearing their hand on their
 * sleeve. Four things keep them from being readable:
 *
 *   1. Bet SIZE is drawn from a fixed distribution and never looks at hand
 *      strength. Sizing is the loudest tell in poker.
 *   2. Strong hands sometimes just check or call (trap), so a check does
 *      not mean weakness.
 *   3. Weak hands sometimes raise (bluff), so a raise does not mean
 *      strength.
 *   4. Every bot gets its own profile, so one read does not cover the
 *      whole table.
 **/

struct BotProfile {
  double aggression = 0.55; // how often a strong hand raises
  double bluff = 0.18;      // how often a weak hand raises anyway
  double trap = 0.25;       // how often a monster refuses to raise
  double looseness = 0.0;   // shifts the continue threshold
  double blockBias = 0.0;   // shifts block frequency
  int trials = 120;         // Monte Carlo samples per decision
};

inline bool chance(double probability) {
  std::uniform_real_distribution<double> roll(0.0, 1.0);
  return roll(rng()) < probability;
}

inline int liveOpponents(const HandState &hand, int seatIndex) {
  int count = 0;
  for (int i = 0; i < hand.seatCount(); ++i) {
    if (i != seatIndex && hand.seats[static_cast<std::size_t>(i)].inHand()) {
      ++count;
    }
  }
  return count;
}

inline int chooseRaiseSize(const HandState &hand, const Game &game,
                           int seatIndex) {
  const int lo = minRaiseTo(hand, game, seatIndex);
  const int hi = maxRaiseTo(hand, game, seatIndex);
  if (hi <= lo)
    return lo;

  std::uniform_int_distribution<int> style(0, 99);
  const int roll = style(rng());
  const int target = roll < 55   ? lo
                     : roll < 85 ? lo + (hi - lo) / 4
                                 : lo + (hi - lo) / 2;
  return std::clamp(target, lo, hi);
}

inline double continueThreshold(const HandState &hand, const Game &game,
                                int seatIndex, const BotProfile &profile) {
  const Seat &seat = hand.seats[static_cast<std::size_t>(seatIndex)];
  const Player &me = game.players[static_cast<std::size_t>(seat.id)];
  const int owed = toCall(hand, seatIndex);

  const double exposure =
      static_cast<double>(owed) / static_cast<double>(std::max(1, me.health));
  return std::clamp(0.32 + exposure * 0.9 - profile.looseness, 0.10, 0.92);
}

inline Action smartAct(const BotProfile &profile, const HandState &hand,
                       const Game &game, int seatIndex) {
  const Seat &seat = hand.seats[static_cast<std::size_t>(seatIndex)];
  const int owed = toCall(hand, seatIndex);
  const bool mayRaise = canRaise(hand, game, seatIndex);

  const double equity =
      handEquity(seat.hole, hand.board.data(), hand.boardCount,
                 std::max(1, liveOpponents(hand, seatIndex)), profile.trials);

  const bool strong = equity > 0.66;
  const bool playable =
      equity >= continueThreshold(hand, game, seatIndex, profile);

  const auto raise = [&] {
    return Action{ActionType::RAISE, chooseRaiseSize(hand, game, seatIndex)};
  };

  if (owed == 0) {
    if (strong && !chance(profile.trap) && chance(profile.aggression) &&
        mayRaise) {
      return raise();
    }
    if (!playable && chance(profile.bluff) && mayRaise)
      return raise();
    return Action{ActionType::CHECK, 0};
  }

  if (strong) {
    if (!chance(profile.trap) && chance(profile.aggression) && mayRaise) {
      return raise();
    }
    return Action{ActionType::CALL, 0};
  }

  if (playable) {
    if (chance(profile.bluff * 0.6) && mayRaise)
      return raise();
    return Action{ActionType::CALL, 0};
  }

  if (chance(profile.bluff) && mayRaise)
    return raise();
  return Action{ActionType::FOLD, 0};
}

inline int smartPickTarget(const BotProfile &, const HandState &hand,
                           const Game &game, int seatIndex) {
  const std::vector<int> targets = targetsFor(hand, game, seatIndex);
  if (targets.empty())
    return -1;

  double total = 0.0;
  std::vector<double> weights;
  weights.reserve(targets.size());
  for (const int seat : targets) {
    const Player &player = game.players[static_cast<std::size_t>(
        hand.seats[static_cast<std::size_t>(seat)].id)];
    double weight = static_cast<double>(punchDamage(hand, seat));
    if (punchDamage(hand, seat) >= player.health)
      weight *= 3.0;
    if (!player.canBlock())
      weight *= 1.6;
    weights.push_back(weight);
    total += weight;
  }

  std::uniform_real_distribution<double> roll(0.0, total);
  double pick = roll(rng());
  for (std::size_t i = 0; i < targets.size(); ++i) {
    pick -= weights[i];
    if (pick <= 0.0)
      return targets[i];
  }
  return targets.back();
}

inline GuardChoice smartGuard(const BotProfile &profile, const HandState &hand,
                              const Game &game, int seatIndex,
                              int /*attacker*/) {
  const Player &me = game.players[static_cast<std::size_t>(
      hand.seats[static_cast<std::size_t>(seatIndex)].id)];
  if (!me.canBlock())
    return GuardChoice::NO_BLOCK;

  const int atRisk = punchDamage(hand, seatIndex);
  const double lethal = atRisk >= me.health ? 0.35 : 0.0;
  const double hurt = 0.30 * static_cast<double>(atRisk) /
                      static_cast<double>(std::max(1, me.health));

  return chance(
             std::clamp(0.22 + hurt + lethal + profile.blockBias, 0.10, 0.80))
             ? GuardChoice::BLOCK
             : GuardChoice::NO_BLOCK;
}

#endif
