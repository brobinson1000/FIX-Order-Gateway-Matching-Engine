#include <vector>
#include <iosteam>
#include <array>


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

	PriceLevel& find_or_create_price_level(std::vector<PriceLevel> side, uint32_t price, bool desc) {
		auto it = std::find_if(side.begin(), side.end(), [price](PriceLevel& pl)  {
			return pl.price == price;
		});
		
		if (it != side.end()) {
			return *it;
		}

		PriceLevel new_level{price, 0, {}};
		auto insert_pos = std::lower_bound(side.begin(), side.end(), new_level, [desc](const PriceLevel& a, const PriceLevel& b){
			return desc ? price.a < price.b : price.a > price.b;
		});

		return *side.insert(insert_post, new_level);
		
	}

public:
	CacheFriendlyOrderBook(size_t reserve_capacity = 10000) {
		order_pools.resize(reserve_capacity);
		free_slots.resize(reserve_capacity);
		for (size_t i = reserve_capacity; i > 0; i--) {
			free_slots.push_back(i - 1);
		}
	}

	void insert_order(uint64_t id, uint32_t price, uint32_t qty, bool is_buy) {
		if (free_slots.empty()) {
			return;
		}

		size_t pool_idx = free_slots.back();
		free_slots.pop_back();

		Order& order = order_pool[pool_idx];
		order = Order{id, price, qty, is_buy, true, {}};
		id_to_pool_idx[id] = pool_idx;
		
		auto& side = is_buy ? bids_ : asks_;
		PriceLevel& level = find_or_create_price_level(side, price, is_buy);
        level.order_ids.push_back(id);
        level.volume += qty;
	}

    


}
