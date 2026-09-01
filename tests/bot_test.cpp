#include "engine.hpp"
#include <cstdio>
#include <vector>

// Does hand strength leak through the visible action?
struct Tell {
  double sumEquity = 0.0;
  int count = 0;
  double avg() const { return count ? sumEquity / count : 0.0; }
};

int main() {
  int fail = 0;

  // 1. Smart bots must beat the naive baseline head to head.
  {
    int smartWins = 0;
    const int GAMES = 300;
    for (int g = 0; g < GAMES; ++g) {
      Game game = createGame("Smart", 2);
      game.players[0].isHuman = false;
      game.button = g % 2;

      AgentTable agents;
      agents.push_back(makeBot(BOT_PROFILES[0])); // seat 0 smart
      agents.push_back(makeNaiveBot());           // seat 1 naive

      Observer silent;
      runGame(game, agents, silent);
      if (!game.players[0].isKnockedOut) ++smartWins;
    }
    const double rate = 100.0 * smartWins / GAMES;
    const bool ok = rate > 55.0;
    if (!ok) ++fail;
    std::printf("smart vs naive win rate : %.1f%%  %s\n", rate,
                ok ? "ok" : "FAIL (not actually smarter)");
  }

  // 2. Raising must not imply strength. Collect the equity behind every
  // action a smart bot takes and check the ranges overlap.
  {
    Tell raised, checked, called, folded;
    BotProfile profile = BOT_PROFILES[0];
    profile.trials = 200;

    for (int trial = 0; trial < 4000; ++trial) {
      Game game = createGame("A", 3);
      for (auto &p : game.players) p.isHuman = false;
      HandState hand = startHand(game);

      // random street
      const int street = trial % 3;
      if (street >= 1) dealFlop(hand);
      if (street >= 2) dealTurn(hand);
      if (street >= 1) beginStreet(hand, game);

      // Half the trials face a live bet, so CALL and FOLD get sampled too.
      if (trial % 2 == 0) {
        applyAction(hand, game, 0, Action{ActionType::RAISE,
                                          minRaiseTo(hand, game, 0)});
      }

      const int seatIndex = 2;
      const Seat &seat = hand.seats[seatIndex];
      const double eq = handEquity(seat.hole, hand.board.data(),
                                   hand.boardCount, 2, 400);

      const Action action = smartAct(profile, hand, game, seatIndex);
      Tell *bucket = action.type == ActionType::RAISE  ? &raised
                     : action.type == ActionType::CHECK ? &checked
                     : action.type == ActionType::CALL  ? &called
                                                        : &folded;
      bucket->sumEquity += eq;
      bucket->count += 1;
    }

    std::printf("\naverage equity behind each action:\n");
    std::printf("  raise  %.3f  (n=%d)\n", raised.avg(), raised.count);
    std::printf("  call   %.3f  (n=%d)\n", called.avg(), called.count);
    std::printf("  check  %.3f  (n=%d)\n", checked.avg(), checked.count);
    std::printf("  fold   %.3f  (n=%d)\n", folded.avg(), folded.count);

    // A raise must not be a reliable signal: the bot has to raise with weak
    // hands often enough that the average stays near the middle.
    const bool bluffs = raised.count > 0 && raised.avg() < 0.72;
    const bool traps = checked.count > 0 && checked.avg() > 0.30;
    const bool callsMixed = called.count > 0;
    const bool foldsWeak = folded.count > 0 && folded.avg() < called.avg();
    if (!bluffs) ++fail;
    if (!traps) ++fail;
    if (!callsMixed) ++fail;
    if (!foldsWeak) ++fail;
    std::printf("  raise range includes weak hands  : %s\n",
                bluffs ? "ok" : "FAIL (raise = strong, readable)");
    std::printf("  check range includes strong hands: %s\n",
                traps ? "ok" : "FAIL (check = weak, readable)");
    std::printf("  calls actually sampled           : %s\n",
                callsMixed ? "ok" : "FAIL (branch never hit)");
    std::printf("  folds are weaker than calls      : %s\n",
                foldsWeak ? "ok" : "FAIL (fold threshold broken)");
  }

  // 3. Bet size must not correlate with hand strength.
  {
    double weakSum = 0, strongSum = 0;
    int weakN = 0, strongN = 0;
    BotProfile profile = BOT_PROFILES[0];

    for (int trial = 0; trial < 6000; ++trial) {
      Game game = createGame("A", 3);
      for (auto &p : game.players) p.isHuman = false;
      HandState hand = startHand(game);
      dealFlop(hand);
      beginStreet(hand, game);

      const int seatIndex = 2;
      const Seat &seat = hand.seats[seatIndex];
      const double eq =
          handEquity(seat.hole, hand.board.data(), hand.boardCount, 2, 200);

      const Action action = smartAct(profile, hand, game, seatIndex);
      if (action.type != ActionType::RAISE) continue;
      if (eq > 0.66) { strongSum += action.raiseTo; ++strongN; }
      else           { weakSum   += action.raiseTo; ++weakN; }
    }

    const double strongAvg = strongN ? strongSum / strongN : 0;
    const double weakAvg = weakN ? weakSum / weakN : 0;
    const double gap = strongAvg > 0 && weakAvg > 0
                           ? std::abs(strongAvg - weakAvg) / strongAvg : 0;
    const bool ok = gap < 0.12;
    if (!ok) ++fail;
    std::printf("\nraise size, strong %.1f HP vs weak %.1f HP (gap %.1f%%) : %s\n",
                strongAvg, weakAvg, gap * 100,
                ok ? "ok" : "FAIL (size is a tell)");
  }

  std::printf("\n%s (%d failures)\n", fail ? "FAILURES" : "all passed", fail);
  return fail != 0;
}
