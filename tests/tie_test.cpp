#include "showdown.hpp"
#include <cstdio>
#include <string>

static Card parse(const std::string &s) {
  const std::string ranks = "A23456789TJQK";
  return Card{static_cast<CardType>(ranks.find(s[0])),
              s[1] == 'c'   ? SuitType::CLUBS
              : s[1] == 'd' ? SuitType::DIAMONDS
              : s[1] == 'h' ? SuitType::HEARTS
                            : SuitType::SPADES};
}

// Trips on board, everyone plays the same 9-9-9-5-4. Dead tie, but a
// redraw CAN break it since hole cards still feed the kickers.
static HandState boardPlaysTie(int seats) {
  HandState hand;
  const char *board[] = {"9c", "9d", "9h", "2s", "3c"};
  for (int i = 0; i < 5; ++i) hand.board[i] = parse(board[i]);
  hand.boardCount = 5;

  const char *holes[] = {"4c","5d","4h","5s","4d","5c","4s","5h"};
  for (int i = 0; i < seats; ++i) {
    Seat s;
    s.id = static_cast<PlayerId>(i);
    s.hole[0] = parse(holes[i * 2]);
    s.hole[1] = parse(holes[i * 2 + 1]);
    s.committed = 20;
    hand.seats.push_back(s);
  }
  shuffleDeck(hand.deck);
  return hand;
}

int main() {
  int fail = 0;

  // 1. Board-plays tie must resolve to exactly one winner via redraw.
  int redrawUsed = 0, multiRound = 0;
  for (int t = 0; t < 20000; ++t) {
    HandState hand = boardPlaysTie(4);
    Showdown sd = resolveShowdown(hand);
    if (sd.winners.size() != 1) ++fail;
    if (!sd.tiebreak.empty()) ++redrawUsed;
    if (sd.tiebreak.size() > 1) ++multiRound;
    if (sd.unresolved) ++fail;
    // board must be untouched by the redraw
    if (hand.boardCount != 5) ++fail;
  }
  std::printf("board-plays ties resolved to 1 winner : %s\n", fail ? "FAIL" : "ok");
  std::printf("  needed a redraw     : %d / 20000\n", redrawUsed);
  std::printf("  needed 2+ redraws   : %d / 20000\n", multiRound);

  // 2. A clear winner must NOT trigger a redraw.
  {
    HandState hand;
    const char *board[] = {"2c","7d","9h","Jc","4s"};
    for (int i = 0; i < 5; ++i) hand.board[i] = parse(board[i]);
    hand.boardCount = 5;
    Seat a; a.id = 0; a.hole[0] = parse("Ac"); a.hole[1] = parse("Ad");
    Seat b; b.id = 1; b.hole[0] = parse("3c"); b.hole[1] = parse("5d");
    hand.seats = {a, b};
    shuffleDeck(hand.deck);
    Showdown sd = resolveShowdown(hand);
    const bool ok = sd.winners.size() == 1 && sd.winners[0] == 0 &&
                    sd.tiebreak.empty();
    if (!ok) ++fail;
    std::printf("clear winner, no redraw               : %s\n", ok ? "ok" : "FAIL");
  }

  // 3. Folded out: uncontested, no cards compared.
  {
    HandState hand = boardPlaysTie(3);
    hand.seats[0].status = SeatStatus::FOLDED;
    hand.seats[2].status = SeatStatus::FOLDED;
    Showdown sd = resolveShowdown(hand);
    const bool ok = !sd.contested && sd.winners.size() == 1 &&
                    sd.winners[0] == 1 && sd.tiebreak.empty();
    if (!ok) ++fail;
    std::printf("folded out, uncontested               : %s\n", ok ? "ok" : "FAIL");
  }

  // 4. Redraw must never deal a card already on the board or in a hand.
  {
    int dup = 0;
    for (int t = 0; t < 20000; ++t) {
      HandState hand = boardPlaysTie(4);
      const std::size_t before = cardsLeft(hand.deck);
      Showdown sd = resolveShowdown(hand);
      std::size_t drawn = 0;
      for (const TieRound &r : sd.tiebreak) drawn += r.hole.size() * 2;
      if (before - cardsLeft(hand.deck) != drawn) ++dup;
    }
    if (dup) ++fail;
    std::printf("redraw pulls from live deck only      : %s\n", dup ? "FAIL" : "ok");
  }

  // 5. Unbreakable board: a royal flush on the table can never be beaten
  // or separated by any redraw. Must still yield exactly one puncher.
  {
    HandState hand;
    const char *board[] = {"As","Ks","Qs","Js","Ts"};
    for (int i = 0; i < 5; ++i) hand.board[i] = parse(board[i]);
    hand.boardCount = 5;
    const char *holes[] = {"2c","3c","2d","3d","2h","3h"};
    for (int i = 0; i < 3; ++i) {
      Seat s2; s2.id = static_cast<PlayerId>(i);
      s2.hole[0] = parse(holes[i*2]); s2.hole[1] = parse(holes[i*2+1]);
      hand.seats.push_back(s2);
    }
    shuffleDeck(hand.deck);
    Showdown sd = resolveShowdown(hand);
    const bool ok = sd.winners.size() == 1 && sd.unresolved &&
                    sd.tiebreak.size() == MAX_TIE_ROUNDS;
    if (!ok) ++fail;
    std::printf("royal-flush board falls back by seat  : %s\n", ok ? "ok" : "FAIL");
  }

  std::printf("\n%s (%d failures)\n", fail ? "FAILURES" : "all passed", fail);
  return fail != 0;
}
