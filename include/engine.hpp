#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "agent.hpp"
#include "betting.hpp"
#include "hand.hpp"
#include <functional>
#include <memory>
#include <vector>

/**
 * @author: { @Override } : 20260831
 * -----
 *  Drives one hand through the street machine:
 *
 *    PREFLOP  bet -> FLOP (3 cards)  bet
 *             -> TURN (4th card)     bet
 *             -> RIVER (5th card)    bet
 *             -> SHOWDOWN
 *
 *  A hand ends early the moment everyone but one player has folded.
 *
 *  The engine does not print. It calls back into an Observer so the render
 *  layer can draw whatever it wants between streets.
 **/

using AgentTable = std::vector<std::unique_ptr<Agent>>;

struct Observer {
  std::function<void(const HandState &, const Game &)> onStreet;
  std::function<void(const HandState &, const Game &, int, const Action &)>
      onAction;
};

inline void runBettingRound(HandState &hand, const Game &game,
                            AgentTable &agents, const Observer &observer) {
  while (!bettingClosed(hand)) {
    const int idx = hand.actor;
    if (idx < 0)
      break;

    Seat &seat = hand.seats[static_cast<std::size_t>(idx)];
    if (!seat.canAct()) {
      const int next = nextActableSeat(hand, (idx + 1) % hand.seatCount());
      if (next < 0)
        break;
      hand.actor = next;
      continue;
    }

    Agent &agent = *agents[static_cast<std::size_t>(seat.id)];
    const Action action = legalize(hand, game, idx, agent.act(hand, game, idx));

    applyAction(hand, game, idx, action);
    if (observer.onAction)
      observer.onAction(hand, game, idx, action);

    const int next = nextActableSeat(hand, (idx + 1) % hand.seatCount());
    if (next < 0)
      break;
    hand.actor = next;
  }
}

inline void playHand(HandState &hand, const Game &game, AgentTable &agents,
                     const Observer &observer) {
  if (observer.onStreet)
    observer.onStreet(hand, game);
  runBettingRound(hand, game, agents, observer);

  const auto nextStreet = [&](void (*deal)(HandState &)) {
    if (hand.liveCount() <= 1)
      return false;
    deal(hand);
    beginStreet(hand, game);
    if (observer.onStreet)
      observer.onStreet(hand, game);
    runBettingRound(hand, game, agents, observer);
    return true;
  };

  if (!nextStreet(dealFlop)) {
    hand.street = Street::DONE;
    return;
  }
  if (!nextStreet(dealTurn)) {
    hand.street = Street::DONE;
    return;
  }
  if (!nextStreet(dealRiver)) {
    hand.street = Street::DONE;
    return;
  }

  hand.street = hand.liveCount() > 1 ? Street::SHOWDOWN : Street::DONE;
  if (observer.onStreet)
    observer.onStreet(hand, game);
}

#endif
