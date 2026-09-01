#include "hand.hpp"
#include <cstdio>
#include <set>

int main() {
  int dupRuns = 0, potBad = 0, sumBad = 0;
  for (int trial = 0; trial < 20000; ++trial) {
    Game game = createGame("Tester", 6);
    game.button = trial % 6;
    HandState hand = startHand(game);
    dealFlop(hand); dealTurn(hand); dealRiver(hand);

    std::set<int> seen;
    int total = 0;
    for (const Seat &s : hand.seats) {
      for (const Card &c : s.hole)
        seen.insert(static_cast<int>(c.suit) * 13 + static_cast<int>(c.type));
      total += s.committed;
    }
    for (int i = 0; i < hand.boardCount; ++i)
      seen.insert(static_cast<int>(hand.board[i].suit) * 13 +
                  static_cast<int>(hand.board[i].type));

    if (seen.size() != 17) ++dupRuns;              // 6*2 hole + 5 board
    if (hand.punchPot() != SMALL_BLIND + BIG_BLIND) ++potBad;
    if (total != hand.punchPot()) ++sumBad;
  }
  std::printf("duplicate-card runs: %d / 20000\n", dupRuns);
  std::printf("wrong pot runs:      %d / 20000\n", potBad);
  std::printf("pot != sum committed:%d / 20000\n", sumBad);

  // heads-up: button posts the small blind
  Game hu = createGame("Tester", 2);
  HandState h = startHand(hu);
  std::printf("heads-up button committed: %d (want %d)\n",
              h.seats[hu.button].committed, SMALL_BLIND);
  return 0;
}
