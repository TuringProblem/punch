#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "agent.hpp"
#include "betting.hpp"
#include "hand.hpp"
#include "punch.hpp"
#include "showdown.hpp"
#include <algorithm>
#include <functional>
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

using AgentTable = std::vector<Agent>;

struct Observer {
  std::function<void(const Game &)> onHandStart;
  std::function<void(const Game &)> onHandEnd;
  std::function<void(const Game &)> onGameOver;
  std::function<void(const HandState &, const Game &)> onStreet;
  std::function<void(const HandState &, const Game &, int, const Action &)>
      onAction;
  std::function<void(const HandState &, const Game &, const PunchOutcome &)>
      onPunch;
  std::function<void(const HandState &, const Game &, const Showdown &)>
      onShowdown;
};

inline PunchOutcome runPunchPhase(HandState &hand, Game &game,
                                  AgentTable &agents, int attacker,
                                  const Observer &observer) {
  hand.street = Street::PUNCH;

  const std::vector<int> targets = targetsFor(hand, game, attacker);
  if (targets.empty())
    return PunchOutcome{};

  const Agent &winner = agents[static_cast<std::size_t>(
      hand.seats[static_cast<std::size_t>(attacker)].id)];
  int target = winner.pickTarget(hand, game, attacker);
  if (std::find(targets.begin(), targets.end(), target) == targets.end()) {
    target = targets.front();
  }

  std::vector<GuardChoice> guards(static_cast<std::size_t>(hand.seatCount()),
                                  GuardChoice::NO_BLOCK);
  for (int i = 0; i < hand.seatCount(); ++i) {
    if (i == attacker)
      continue;
    const Agent &agent = agents[static_cast<std::size_t>(
        hand.seats[static_cast<std::size_t>(i)].id)];
    guards[static_cast<std::size_t>(i)] = agent.guard(hand, game, i, attacker);
  }

  const PunchOutcome out = resolvePunch(hand, game, attacker, target, guards);
  applyPunch(game, hand, out);

  if (observer.onPunch)
    observer.onPunch(hand, game, out);
  return out;
}

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

    const Agent &agent = agents[static_cast<std::size_t>(seat.id)];
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

inline void advanceButton(Game &game) {
  const int live = game.playersRemaining();
  if (live <= 0)
    return;
  game.button = (game.button + 1) % live;
}

inline const Player *lastStanding(const Game &game) {
  for (const Player &player : game.players) {
    if (!player.isKnockedOut)
      return &player;
  }
  return nullptr;
}

inline void runGame(Game &game, AgentTable &agents, const Observer &observer) {
  while (!game.isOver()) {
    ++game.handNumber;
    if (game.button >= game.playersRemaining())
      game.button = 0;
    if (observer.onHandStart)
      observer.onHandStart(game);

    HandState hand = startHand(game);
    playHand(hand, game, agents, observer);

    const Showdown result = resolveShowdown(hand);
    if (observer.onShowdown)
      observer.onShowdown(hand, game, result);

    if (!result.winners.empty()) {
      runPunchPhase(hand, game, agents, result.winners.front(), observer);
    }

    if (observer.onHandEnd)
      observer.onHandEnd(game);
    advanceButton(game);
  }

  if (observer.onGameOver)
    observer.onGameOver(game);
}

#endif
