#ifndef BETTING_HPP
#define BETTING_HPP

#include "hand.hpp"
#include <algorithm>

/**
 * @author: { @Override } : 20260831
 * -----
 *  One betting round. Same rules as Texas Hold'em, except the pot is
 *  denominated in HP instead of chips, and betting does not spend health,
 *  it only puts it at risk.
 *
 *  A round closes when every player who can still act has acted and has
 *  matched the current bet. Any raise reopens the action for everyone.
 **/

enum class ActionType { FOLD, CHECK, CALL, RAISE };

struct Action {
  ActionType type = ActionType::FOLD;
  int raiseTo = 0; // for RAISE: the street total to raise up to
};

// HP this seat still owes to stay in the hand this street.
inline int toCall(const HandState &hand, int seatIndex) noexcept {
  const Seat &seat = hand.seats[static_cast<std::size_t>(seatIndex)];
  return std::max(0, hand.currentBet - seat.streetCommitted);
}

// Largest street total this seat can reach, capped by remaining health.
inline int maxRaiseTo(const HandState &hand, const Game &game,
                      int seatIndex) noexcept {
  const Seat &seat = hand.seats[static_cast<std::size_t>(seatIndex)];
  const Player &player = game.players[static_cast<std::size_t>(seat.id)];
  return player.health - seat.committed + seat.streetCommitted;
}

// Smallest legal raise. A raise must lift the bet by at least the size of
// the previous raise, same as real poker.
inline int minRaiseTo(const HandState &hand, const Game &game,
                      int seatIndex) noexcept {
  const int wanted = hand.currentBet + std::max(hand.lastRaiseSize, game.bigBlind);
  return std::min(wanted, maxRaiseTo(hand, game, seatIndex));
}

inline bool canCheck(const HandState &hand, int seatIndex) noexcept {
  return toCall(hand, seatIndex) == 0;
}

// False when the seat is already all-in at the current bet, so there is
// nothing left to raise with.
inline bool canRaise(const HandState &hand, const Game &game,
                     int seatIndex) noexcept {
  return maxRaiseTo(hand, game, seatIndex) > hand.currentBet;
}

inline void applyAction(HandState &hand, const Game &game, int seatIndex,
                        const Action &action) {
  Seat &seat = hand.seats[static_cast<std::size_t>(seatIndex)];
  seat.hasActed = true;

  switch (action.type) {
  case ActionType::FOLD:
    seat.status = SeatStatus::FOLDED;
    break;

  case ActionType::CHECK:
    break;

  case ActionType::CALL:
    commit(hand, game, seatIndex, toCall(hand, seatIndex));
    break;

  case ActionType::RAISE: {
    const int target = std::clamp(action.raiseTo,
                                  minRaiseTo(hand, game, seatIndex),
                                  maxRaiseTo(hand, game, seatIndex));
    commit(hand, game, seatIndex, target - seat.streetCommitted);

    hand.lastRaiseSize = seat.streetCommitted - hand.currentBet;
    hand.currentBet = seat.streetCommitted;
    hand.lastAggressor = seatIndex;

    // A raise reopens the action: everyone else owes another decision.
    for (int i = 0; i < hand.seatCount(); ++i) {
      if (i != seatIndex && hand.seats[static_cast<std::size_t>(i)].canAct()) {
        hand.seats[static_cast<std::size_t>(i)].hasActed = false;
      }
    }
    break;
  }
  }
}

inline bool bettingClosed(const HandState &hand) noexcept {
  if (hand.liveCount() <= 1) return true;

  for (const Seat &seat : hand.seats) {
    if (!seat.canAct()) continue;
    if (!seat.hasActed) return false;
    if (seat.streetCommitted < hand.currentBet) return false;
  }
  return true;
}

// Map an illegal action onto the nearest legal one, so a confused agent can
// never stall a betting round.
inline Action legalize(const HandState &hand, const Game &game, int seatIndex,
                       Action action) {
  if (action.type == ActionType::CHECK && !canCheck(hand, seatIndex)) {
    action.type = ActionType::CALL;
  }
  if (action.type == ActionType::RAISE && !canRaise(hand, game, seatIndex)) {
    action.type = toCall(hand, seatIndex) > 0 ? ActionType::CALL
                                              : ActionType::CHECK;
  }
  if (action.type == ActionType::RAISE) {
    action.raiseTo = std::clamp(action.raiseTo,
                                minRaiseTo(hand, game, seatIndex),
                                maxRaiseTo(hand, game, seatIndex));
  }
  return action;
}

#endif // BETTING_HPP
