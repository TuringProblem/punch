#include "engine.hpp"
#include <cstdio>
#include <set>

int main() {
  int potMismatch = 0, overCommit = 0, dupCards = 0, noWinner = 0;
  int showdowns = 0, foldouts = 0;

  for (int trial = 0; trial < 30000; ++trial) {
    Game game = createGame("Tester", 2 + (trial % 5));
    game.button = trial % game.seatCount();
    for (auto &p : game.players) p.health = 40 + (trial % 61); // varied stacks

    AgentTable agents;
    for (int i = 0; i < game.seatCount(); ++i)
      agents.push_back(makeBot());

    HandState hand = startHand(game);
    Observer obs;                       // silent
    playHand(hand, game, agents, obs);

    std::set<int> seen;
    for (const Seat &s : hand.seats) {
      if (s.committed > game.players[s.id].health) ++overCommit;
      for (const Card &c : s.hole)
        seen.insert(int(c.suit) * 13 + int(c.type));
    }
    for (int i = 0; i < hand.boardCount; ++i)
      seen.insert(int(hand.board[i].suit) * 13 + int(hand.board[i].type));

    int maxCommitted = 0, cap = 0;
    for (const Seat &s2 : hand.seats) {
      if (s2.committed > maxCommitted) maxCommitted = s2.committed;
      if (game.players[s2.id].health > cap) cap = game.players[s2.id].health;
    }
    if (hand.punchPot() != maxCommitted) ++potMismatch;
    if (hand.punchPot() > cap) ++overCommit;
    if (seen.size() != size_t(hand.seatCount() * 2 + hand.boardCount)) ++dupCards;
    if (hand.liveCount() < 1) ++noWinner;

    if (hand.street == Street::SHOWDOWN) ++showdowns; else ++foldouts;
  }

  std::printf("pot != stake level   : %d / 30000\n", potMismatch);
  std::printf("pot or commit > health: %d / 30000\n", overCommit);
  std::printf("duplicate cards      : %d / 30000\n", dupCards);
  std::printf("hands with 0 players : %d / 30000\n", noWinner);
  std::printf("reached showdown     : %d\n", showdowns);
  std::printf("ended by folds       : %d\n", foldouts);
  return 0;
}
