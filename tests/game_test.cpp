#include "engine.hpp"
#include <cstdio>

int main() {
  int fail = 0;
  long long totalHands = 0, maxHands = 0;
  int badSurvivors = 0, badHealth = 0, badBlocks = 0, knockedAlive = 0;

  const int GAMES = 4000;
  for (int g = 0; g < GAMES; ++g) {
    Game game = createGame("Bot0", 2 + (g % 5));
    game.players[0].isHuman = false;

    AgentTable agents;
    for (int i = 0; i < game.seatCount(); ++i) agents.push_back(makeBot());

    Observer silent;
    runGame(game, agents, silent);

    totalHands += game.handNumber;
    if (game.handNumber > maxHands) maxHands = game.handNumber;

    int standing = 0;
    for (const Player &p : game.players) {
      if (!p.isKnockedOut) ++standing;
      if (p.health < 0 || p.health > START_HEALTH) ++badHealth;
      if (p.blocks < 0 || p.blocks > START_BLOCKS) ++badBlocks;
      if (p.isKnockedOut && p.health > 0) ++knockedAlive;
      if (!p.isKnockedOut && p.health == 0) ++knockedAlive;
    }
    if (standing != 1) ++badSurvivors;
  }

  fail += badSurvivors + badHealth + badBlocks + knockedAlive;
  std::printf("games completed          : %d\n", GAMES);
  std::printf("exactly one survivor     : %s\n", badSurvivors ? "FAIL" : "ok");
  std::printf("health within 0..100     : %s\n", badHealth ? "FAIL" : "ok");
  std::printf("blocks within 0..4       : %s\n", badBlocks ? "FAIL" : "ok");
  std::printf("knockout flag consistent : %s\n", knockedAlive ? "FAIL" : "ok");
  std::printf("avg hands per game       : %.1f\n", double(totalHands) / GAMES);
  std::printf("longest game             : %lld hands\n", maxHands);
  std::printf("\n%s (%d failures)\n", fail ? "FAILURES" : "all passed", fail);
  return fail != 0;
}
