#include <gtest/gtest.h>
#include "OrderBook.h"

// GROUPNAME | TESTNAME
TEST(OrderBook, EmptyBookMatchesNothing) {
    CacheFriendlyOrderBook book(100);
    uint32_t left = book.match_order(1, 100, 10, true);
    EXPECT_EQ(left, 10u); // variable and its expected value
}

TEST(OrderBook, OrderBookMatch) {
    CacheFriendlyOrderBook book(100);
    ASSERT_TRUE(book.insert_order(1, 100, 10, true).has_value());
    uint32_t left = book.match_order(2, 100, 10, false);
    EXPECT_EQ(left, 0u);
}

TEST(OrderBook, OrderBookDelete) {
    CacheFriendlyOrderBook book(100);
    ASSERT_TRUE(book.insert_order(1, 100, 10, true).has_value());
    auto removed = book.delete_order(1);
    ASSERT_TRUE(removed.has_value());
    EXPECT_EQ(*removed, 1u);
    uint32_t left = book.match_order(2, 100, 10, false);
    EXPECT_EQ(left, 10u);   // buy is gone, so nothing fills
}

TEST(OrderBook, OrderBookMax) {
    constexpr uint64_t N = 10000;
    CacheFriendlyOrderBook book(N);
    for (uint64_t i{}; i < N; i++) {
        ASSERT_TRUE(book.insert_order(i, 100, 10, true).has_value()) << "Failed at i = " << i;
    }
    EXPECT_FALSE(book.insert_order(N, 100, 10, true).has_value());

    ASSERT_TRUE(book.delete_order(0).has_value());
    EXPECT_TRUE(book.insert_order(N, 100, 10, true).has_value());
}

