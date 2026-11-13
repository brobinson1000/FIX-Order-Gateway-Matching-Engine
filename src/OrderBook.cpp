#include "OrderBook.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <optional>

#include "Enums.h"
#include "Order.h"
#include "Using.h"

PriceLevel& CacheFriendlyOrderBook::find_or_create_price_level(
    std::vector<PriceLevel>& side, uint32_t price, bool desc) {
  auto it = std::find_if(side.begin(), side.end(),
                         [price](PriceLevel& pl) { return pl.price == price; });

  if (it != side.end()) {
    return *it;
  }

  PriceLevel new_level{price, 0, {}};
  auto insert_pos =
      std::lower_bound(side.begin(), side.end(), new_level,
                       [desc](const PriceLevel& a, const PriceLevel& b) {
                         return desc ? a.price > b.price : a.price < b.price;
                       });

  return *side.insert(insert_pos, new_level);
}

CacheFriendlyOrderBook::CacheFriendlyOrderBook(
    size_t reserve_capacity) {
  order_pools.resize(reserve_capacity);
  free_slots.reserve(reserve_capacity);
  for (size_t i = reserve_capacity; i > 0; i--) {
    free_slots.push_back(i - 1);
  }
}

std::optional<uint32_t> CacheFriendlyOrderBook::match_order([[maybe_unused]] uint64_t id, uint32_t price,
                                             uint32_t qty, bool is_buy) {
  auto& opp = is_buy ? asks_ : bids_;

  while (qty > 0 && !opp.empty()) {
    PriceLevel& level = opp.front();
    bool crosses = is_buy ? level.price <= price : level.price >= price;
    if (!crosses) return std::nullopt;

    while (qty > 0 && !level.order_ids.empty()) {
      uint64_t maker_id = level.order_ids.front();
      size_t idx = id_to_pool_idx[maker_id];
      Order& maker = order_pools[idx];

      uint32_t fill = std::min(maker.remainingQuantity_, qty);
      qty -= fill;
      maker.remainingQuantity_ -= fill;
      level.volume -= fill;

      if (maker.remainingQuantity_ == 0) {
        maker.status_ = Status::Filled;
        level.order_ids.erase(level.order_ids.begin());
        id_to_pool_idx.erase(maker_id);
        free_slots.push_back(idx);
      } else {
        maker.status_ = Status::PartiallyFilled;
      }
    }

    if (level.order_ids.empty()) opp.erase(opp.begin());
  }
  return qty;
}

void CacheFriendlyOrderBook::add_order(uint64_t id, uint32_t price,
                                       uint32_t qty, bool is_buy) {
  qty = match_order(id, price, qty, is_buy);

  if (qty > 0) insert_order(id, price, qty, is_buy);
}

std::optional<size_t> CacheFriendlyOrderBook::insert_order(uint64_t id, uint32_t price,
                                          uint32_t qty, bool is_buy) {
  if (free_slots.empty()) {
    return std::nullopt;
  }

  if (id_to_pool_idx.contains(id)) {
    return std::nullopt; 
  }

  size_t pool_idx = free_slots.back();
  free_slots.pop_back();

  Order& order = order_pools[pool_idx];
  order = Order{id, qty, qty, price, TimeInForce::GTC, Status::New, Type::Limit,
                is_buy ? Side::Buy : Side::Sell};
  id_to_pool_idx[id] = pool_idx;

  auto& side = is_buy ? bids_ : asks_;
  PriceLevel& level = find_or_create_price_level(side, price, is_buy);
  level.order_ids.push_back(id);
  level.volume += qty;
  return pool_idx;
}

std::optional<uint64_t> CacheFriendlyOrderBook::delete_order(uint64_t id) {
  auto lookup_id = id_to_pool_idx.find(id);
  if (lookup_id == id_to_pool_idx.end()) return std::nullopt;

  size_t pool_idx = lookup_id->second;
  Order& order = order_pools[pool_idx];

  if (order.status_ == Status::Cancelled) return std::nullopt;
  order.status_ = Status::Cancelled;

  auto& side = (order.side_ == Side::Buy) ? bids_ : asks_;
  for (auto level_it = side.begin(); level_it != side.end(); ++level_it) {
    auto& level = *level_it;
    if (level.price == order.price_) {
      auto it = std::find(level.order_ids.begin(), level.order_ids.end(), id);
      if (it != level.order_ids.end()) {
        level.order_ids.erase(it);
        level.volume -= order.remainingQuantity_;
      }
      if (level.order_ids.empty()) side.erase(level_it);
      break;
    }
  }
  id_to_pool_idx.erase(lookup_id);
  free_slots.push_back(pool_idx);
  return id;
}
