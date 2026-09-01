#ifndef RULES_HPP
#define RULES_HPP

/**
 * @author: { @Override } : 20260831
 * -----
 *  Every tunable number in PUNCH lives here. Nothing else in the codebase
 *  should hardcode a health, block, or blind value.
 *
 *  see the docs/IMPLEMENTATION_DESIGN.md for more details
 **/

inline constexpr int START_HEALTH = 100;
inline constexpr int START_BLOCKS = 4;
inline constexpr int SMALL_BLIND = 5;
inline constexpr int BIG_BLIND = 10;
inline constexpr int MIN_PUNCH = 10;
inline constexpr int MIN_SEATS = 2;
inline constexpr int MAX_SEATS = 6;

#endif
