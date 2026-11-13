#pragma once

#include <cstdint>
#include <vector>
#include <iostream>
#include <array>
#include <unordered_map>
#include <optional>
#include "Enums.h"
#include "Order.h"
#include "Using.h"

inline bool market_closed = false;

struct PriceLevel {
	uint32_t price;
	uint32_t volume{};
	std::vector<uint64_t> order_ids;
};


class CacheFriendlyOrderBook {
private:
	std::vector<Order> order_pools;
	std::vector<size_t> free_slots;
	std::unordered_map<uint64_t, size_t> id_to_pool_idx;

	std::vector<PriceLevel> bids_;
	std::vector<PriceLevel> asks_;

	PriceLevel& find_or_create_price_level(std::vector<PriceLevel>& side, uint32_t price, bool desc);

public:
	CacheFriendlyOrderBook(size_t reserve_capacity = 10000);

    std::optional<uint32_t> match_order(uint64_t id, uint32_t price, uint32_t qty, bool is_buy);

    void add_order(uint64_t id, uint32_t price, uint32_t qty, bool is_buy);

	std::optional<size_t> insert_order(uint64_t id, uint32_t price, uint32_t qty, bool is_buy);

    std::optional<uint64_t> delete_order(uint64_t id);

};
