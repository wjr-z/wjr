#ifndef WJR_UNITS_DETAIL_HPP__
#define WJR_UNITS_DETAIL_HPP__

#include <ctime>
#include <functional>
#include <gtest/gtest.h>
#include <random>

static std::mt19937_64 __mt_rand(std::time(0));
static auto mt_rand = std::ref(__mt_rand);

#endif // WJR_UNITS_DETAIL_HPP__