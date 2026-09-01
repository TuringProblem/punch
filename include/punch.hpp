#ifndef PUNCH_HPP
#define PUNCH_HPP

#include "hand.hpp"
#include "rules.hpp"
#include <algorithm>
#include <vector>

/**
 * @author: { @Override } : 20260831
 *
 * Winner picks one target. Everyone else secretly picks block or no block
 * at the same time, so nobody knows who is getting hit until the reveal.
 *
 *   punched + blocked     -> 0 damage, block kept
 *   punched + no block    -> full damage
 *   not punched + blocked -> block burned
 *   not punched + no block-> nothing
 **/

enum class GuardChoice { BLOCK, NO_BLOCK };

struct PunchOutcome {
  int attacker = -1;
  int target = -1;
  int damage = 0;
  bool landed = false;
  std::vector<GuardChoice> guards;
  std::vector<int> blockDelta;
  std::vector<bool> knockedOut;
};

inline int punchDamage(const HandState &hand, int seatIndex) {
  return std::max(MIN_PUNCH,
                  hand.seats[static_cast<std::size_t>(seatIndex)].committed);
}

inline bool canBeTargeted(const HandState &hand, const Game &game, int attacker,
                          int seatIndex) {
  if (seatIndex == attacker)
    return false;
  const Seat &seat = hand.seats[static_cast<std::size_t>(seatIndex)];
  return !game.players[static_cast<std::size_t>(seat.id)].isKnockedOut;
}

inline std::vector<int> targetsFor(const HandState &hand, const Game &game,
                                   int attacker) {
  std::vector<int> targets;
  for (int i = 0; i < hand.seatCount(); ++i) {
    if (canBeTargeted(hand, game, attacker, i))
      targets.push_back(i);
  }
  return targets;
}

inline PunchOutcome resolvePunch(const HandState &hand, const Game &game,
                                 int attacker, int target,
                                 const std::vector<GuardChoice> &guards) {
  const auto count = static_cast<std::size_t>(hand.seatCount());

  PunchOutcome out;
  out.attacker = attacker;
  out.target = target;
  out.guards = guards;
  out.blockDelta.assign(count, 0);
  out.knockedOut.assign(count, false);

  for (std::size_t i = 0; i < count; ++i) {
    if (static_cast<int>(i) == attacker)
      continue;

    const Player &player =
        game.players[static_cast<std::size_t>(hand.seats[i].id)];

    const GuardChoice guard =
        player.canBlock() ? guards[i] : GuardChoice::NO_BLOCK;
    out.guards[i] = guard;

    if (static_cast<int>(i) == target) {
      if (guard == GuardChoice::BLOCK)
        continue;
      out.landed = true;
      out.damage = punchDamage(hand, target);
      out.knockedOut[i] = player.health - out.damage <= 0;
    } else if (guard == GuardChoice::BLOCK) {
      out.blockDelta[i] = -1;
    }
  }

  return out;
}

inline void applyPunch(Game &game, const HandState &hand,
                       const PunchOutcome &out) {
  for (std::size_t i = 0; i < hand.seats.size(); ++i) {
    Player &player = game.players[static_cast<std::size_t>(hand.seats[i].id)];

    player.blocks = std::max(0, player.blocks + out.blockDelta[i]);

    if (out.landed && static_cast<int>(i) == out.target) {
      player.health = std::max(0, player.health - out.damage);
      if (player.health == 0)
        player.isKnockedOut = true;
    }
  }
}

#endif
