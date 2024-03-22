#include <gtest/gtest.h>
#include <random>

#include <wjr/assert.hpp>

static std::mt19937_64 __mt_rand(time(0));
static auto mt_rand = std::ref(__mt_rand);