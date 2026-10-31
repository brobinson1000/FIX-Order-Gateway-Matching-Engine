#include <vector>
#include <iosteam>
#include <array>

bool market_closed = false;

enum class type {
    Limit,
    Market,
};

enum class time_in_force {
    GTC, IOC, FOK, Day,
};

enum class status {
    Filled,
    PartiallyFilled,
    New,
    Cancelled, 
    Rejected,
};



struct Order {
    bool side;
    char[2] status;
    uint64_t order_id;
    uint32_t original_qty;
    uint32_t filled_qty;
    uint32_t price;
};


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

    void match_order(uint64_t id, uint32_t price, uint32_t qty, bool is_buy) {
        auto& opp = is_buy ? asks_ : bids_;

        while(qty > 0 && !opp.empty()) {
            PriceLevel& level = opp.front();
            bool crosses = is_buy ? level.price <= price : level.price >= price;
            if (!crosses) break;

            while (qty > 0 && !level.order_ids.empty()) {
                uint64_t maker_id = level.order_ids.front();
                size_t idx = id_to_pool_idx[maker_id];
                Order& maker = order_pool[idx];

                uint32_t fill = std::min(maker.qty, qty);
                qty -= fill;
                maker.qty -= fill;
                level.volume -= fill;

                if (maker_qty == 0) {
                    level.order_ids.pop_front();
                    id_to_pool_idx.erase(maker_id);
                    free_slots.push_back(idx);
                }

            }

            if (level.order_ids.empty()) opp.erase(opp.begin());

        }
        return qty;
    }

    void add_order(uint64_t id, uint32_t price, uint32_t qty, bool is_buy) {
        qty = match_order(id, price, qty, is_buy);

        if (qty > 0) insert_order(id, price, qty, is_buy);
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

    void delete_order(uint64_t id) {
        auto lookup_id = id_to_pool_idx[id];
        if (lookup_id == id_to_pool_idx.end()) return;

        size_t pool_idx = lookup_id->second;
        Order& order = order_pool[pool_idx];

        if(!order.is_active) return;
        order.is_active = false;

        auto& side = order.is_buy() : bids_ ? asks_;
        for (auto& level : side) {
            if (level.price == order.price) {
                auto it = std::find(level.order_ids.begin(), level.order_ids.end(), id);
                if (it != level.order_ids.end()) {
                    level.order_ids.erase(it);
                    level.volume -= order.quantity;
                }
                break;
            }
        }
        id_to_pool_idx.erase(lookup_id);
        free_slots.push_back(pool_idx);
    }




}


