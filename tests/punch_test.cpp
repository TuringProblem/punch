#include "punch.hpp"
#include <cstdio>

static int fail = 0;
static void check(const char *label, bool ok) {
  if (!ok) ++fail;
  std::printf("  %-42s %s\n", label, ok ? "ok" : "FAIL");
}

struct Fixture {
  Game game;
  HandState hand;
};

static Fixture make(int seats, int committed) {
  Fixture f;
  f.game = createGame("You", seats);
  for (int i = 0; i < seats; ++i) {
    Seat s;
    s.id = static_cast<PlayerId>(i);
    s.committed = committed;
    f.hand.seats.push_back(s);
  }
  return f;
}

int main() {
  std::printf("resolution matrix (attacker seat 0, target seat 1):\n");

  // punched + blocked -> no damage, block kept
  {
    Fixture f = make(3, 40);
    std::vector<GuardChoice> g(3, GuardChoice::NO_BLOCK);
    g[1] = GuardChoice::BLOCK;
    PunchOutcome o = resolvePunch(f.hand, f.game, 0, 1, g);
    applyPunch(f.game, f.hand, o);
    check("punched + blocked: no damage", !o.landed && o.damage == 0);
    check("punched + blocked: block kept", f.game.players[1].blocks == START_BLOCKS);
    check("punched + blocked: full health", f.game.players[1].health == START_HEALTH);
  }

  // punched + no block -> full damage
  {
    Fixture f = make(3, 40);
    std::vector<GuardChoice> g(3, GuardChoice::NO_BLOCK);
    PunchOutcome o = resolvePunch(f.hand, f.game, 0, 1, g);
    applyPunch(f.game, f.hand, o);
    check("punched + open: takes committed", o.landed && o.damage == 40);
    check("punched + open: health 100-40", f.game.players[1].health == 60);
    check("punched + open: blocks unchanged", f.game.players[1].blocks == START_BLOCKS);
  }

  // not punched + blocked -> block burned
  {
    Fixture f = make(3, 40);
    std::vector<GuardChoice> g(3, GuardChoice::NO_BLOCK);
    g[2] = GuardChoice::BLOCK;
    PunchOutcome o = resolvePunch(f.hand, f.game, 0, 1, g);
    applyPunch(f.game, f.hand, o);
    check("wasted block: -1 block", f.game.players[2].blocks == START_BLOCKS - 1);
    check("wasted block: no damage", f.game.players[2].health == START_HEALTH);
  }

  // not punched + no block -> nothing
  {
    Fixture f = make(3, 40);
    std::vector<GuardChoice> g(3, GuardChoice::NO_BLOCK);
    PunchOutcome o = resolvePunch(f.hand, f.game, 0, 1, g);
    applyPunch(f.game, f.hand, o);
    check("bystander open: untouched",
          f.game.players[2].blocks == START_BLOCKS &&
          f.game.players[2].health == START_HEALTH);
  }

  std::printf("\nedges:\n");

  // 10 HP floor for a player who committed nothing
  {
    Fixture f = make(3, 0);
    std::vector<GuardChoice> g(3, GuardChoice::NO_BLOCK);
    PunchOutcome o = resolvePunch(f.hand, f.game, 0, 1, g);
    check("zero committed still hits for 10", o.damage == MIN_PUNCH);
  }

  // no blocks left: BLOCK request is ignored
  {
    Fixture f = make(3, 40);
    f.game.players[1].blocks = 0;
    std::vector<GuardChoice> g(3, GuardChoice::BLOCK);
    PunchOutcome o = resolvePunch(f.hand, f.game, 0, 1, g);
    applyPunch(f.game, f.hand, o);
    check("no blocks: cannot block", o.landed && o.damage == 40);
    check("no blocks: never goes negative", f.game.players[1].blocks == 0);
  }

  // knockout
  {
    Fixture f = make(3, 40);
    f.game.players[1].health = 30;
    std::vector<GuardChoice> g(3, GuardChoice::NO_BLOCK);
    PunchOutcome o = resolvePunch(f.hand, f.game, 0, 1, g);
    applyPunch(f.game, f.hand, o);
    check("lethal punch flags knockout", o.knockedOut[1]);
    check("health floors at 0", f.game.players[1].health == 0);
    check("player marked knocked out", f.game.players[1].isKnockedOut);
  }

  // attacker is never hurt and never spends a block
  {
    Fixture f = make(4, 40);
    std::vector<GuardChoice> g(4, GuardChoice::BLOCK);
    g[2] = GuardChoice::NO_BLOCK; // target stands still, everyone else blocks
    PunchOutcome o = resolvePunch(f.hand, f.game, 0, 2, g);
    applyPunch(f.game, f.hand, o);
    check("attacker untouched",
          f.game.players[0].health == START_HEALTH &&
          f.game.players[0].blocks == START_BLOCKS);
    check("exactly one target takes damage",
          f.game.players[2].health == 60 &&
          f.game.players[1].health == START_HEALTH &&
          f.game.players[3].health == START_HEALTH);
    check("other blockers burn one each",
          f.game.players[1].blocks == 3 && f.game.players[3].blocks == 3);
  }

  // attacker cannot target self, knocked out players are not targets
  {
    Fixture f = make(4, 40);
    f.game.players[2].isKnockedOut = true;
    std::vector<int> t = targetsFor(f.hand, f.game, 0);
    check("self excluded from targets",
          std::find(t.begin(), t.end(), 0) == t.end());
    check("knocked out excluded from targets",
          std::find(t.begin(), t.end(), 2) == t.end());
    check("remaining seats targetable", t.size() == 2);
  }

  std::printf("\n%s (%d failures)\n", fail ? "FAILURES" : "all passed", fail);
  return fail != 0;
}
