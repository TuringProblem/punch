#include "deck.hpp"
#include "handrank.hpp"
#include <cstdio>
#include <string>

static Card parse(const std::string &s) {
  const std::string ranks = "A23456789TJQK";
  CardType t = static_cast<CardType>(ranks.find(s[0]));
  SuitType u = s[1] == 'c' ? SuitType::CLUBS
             : s[1] == 'd' ? SuitType::DIAMONDS
             : s[1] == 'h' ? SuitType::HEARTS
                           : SuitType::SPADES;
  return Card{t, u};
}

static HandRank ev(const char *a, const char *b, const char *c, const char *d,
                   const char *e, const char *f, const char *g) {
  return evaluate7({parse(a), parse(b), parse(c), parse(d), parse(e),
                    parse(f), parse(g)});
}

static int failures = 0;
static void expect(const char *label, Category got, Category want) {
  const bool ok = got == want;
  if (!ok) ++failures;
  std::printf("  %-22s %-17s %s\n", label, categoryName(got).data(),
              ok ? "ok" : "FAIL");
}

int main() {
  std::printf("known hands:\n");
  expect("royal flush",   ev("As","Ks","Qs","Js","Ts","2c","7d").cat, Category::STRAIGHT_FLUSH);
  expect("wheel str flush",ev("As","2s","3s","4s","5s","Kc","7d").cat, Category::STRAIGHT_FLUSH);
  expect("quads",         ev("9c","9d","9h","9s","2c","7d","Ks").cat, Category::QUADS);
  expect("full house",    ev("9c","9d","9h","2s","2c","7d","Ks").cat, Category::FULL_HOUSE);
  expect("trips+trips=boat",ev("9c","9d","9h","2s","2c","2d","Ks").cat, Category::FULL_HOUSE);
  expect("flush",         ev("2s","5s","9s","Js","Ks","7d","3c").cat, Category::FLUSH);
  expect("straight",      ev("5c","6d","7h","8s","9c","Kd","2s").cat, Category::STRAIGHT);
  expect("wheel straight", ev("Ac","2d","3h","4s","5c","Kd","9s").cat, Category::STRAIGHT);
  expect("trips",         ev("9c","9d","9h","2s","5c","7d","Ks").cat, Category::TRIPS);
  expect("two pair",      ev("9c","9d","5h","5s","2c","7d","Ks").cat, Category::TWO_PAIR);
  expect("pair",          ev("9c","9d","5h","3s","2c","7d","Ks").cat, Category::PAIR);
  expect("high card",     ev("9c","Jd","5h","3s","2c","7d","Ks").cat, Category::HIGH_CARD);

  std::printf("\nordering:\n");
  struct { const char *label; bool ok; } checks[] = {
    {"boat beats flush",   ev("9c","9d","9h","2s","2c","7d","Ks") > ev("2s","5s","9s","Js","Ks","7d","3c")},
    {"flush beats straight",ev("2s","5s","9s","Js","Ks","7d","3c") > ev("5c","6d","7h","8s","9c","Kd","2s")},
    {"ace-high > king-high",ev("Ac","Jd","5h","3s","2c","7d","9s") > ev("Kc","Jd","5h","3s","2c","7d","9s")},
    {"kicker decides pair", ev("9c","9d","Ah","3s","2c","7d","5s") > ev("9c","9h","Kh","3s","2c","7d","5s")},
    {"wheel < 6-high str",  ev("Ac","2d","3h","4s","5c","Kd","9s") < ev("2d","3h","4s","5c","6c","Kd","9s")},
    {"same hand ties",      ev("9c","9d","Ah","3s","2c","7d","5s") == ev("9h","9s","Ac","3d","2h","7c","5d")},
  };
  for (auto &c : checks) {
    if (!c.ok) ++failures;
    std::printf("  %-22s %s\n", c.label, c.ok ? "ok" : "FAIL");
  }

  // Category frequencies over random 7-card hands vs published Hold'em odds.
  std::printf("\nfrequencies over 2,000,000 random 7-card hands:\n");
  const double want[] = {17.4, 43.8, 23.5, 4.83, 4.62, 3.03, 2.60, 0.168, 0.0311};
  long long hits[9] = {};
  const int N = 2000000;
  for (int i = 0; i < N; ++i) {
    Deck deck = makeDeck();
    shuffleDeck(deck);
    std::array<Card, 7> h;
    for (auto &c : h) c = dealCard(deck);
    ++hits[static_cast<int>(evaluate7(h).cat)];
  }
  for (int i = 8; i >= 0; --i) {
    const double got = 100.0 * double(hits[i]) / N;
    const double drift = got - want[i];
    const bool ok = (want[i] > 1.0) ? (drift > -0.35 && drift < 0.35)
                                    : (drift > -0.05 && drift < 0.05);
    if (!ok) ++failures;
    std::printf("  %-17s got %7.4f%%  want %7.4f%%  %s\n",
                categoryName(static_cast<Category>(i)).data(), got, want[i],
                ok ? "ok" : "FAIL");
  }

  std::printf("\n%s (%d failures)\n", failures ? "FAILURES" : "all passed", failures);
  return failures != 0;
}
