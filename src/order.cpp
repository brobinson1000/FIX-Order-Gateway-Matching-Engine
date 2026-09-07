#include <string>
#include <cstdint>
#include <chrono>
#include "using.h"
#include "enums.h"


class OrderBook {
    public:
        OrderBook(const OrderBook&) = delete;
        OrderBook& operator = (const OrderBook&) = delete;
        OrderBook(OrderBook&&) = delete;
        OrderBook& operator = ( const OrderBook&& ) = delete;

        static OrderBook& get() {
            static OrderBook instance;
            return instance;
        }

        void appendOrder(const Order& incoming) {

            std::scoped_lock<std::mutex> ordersLock{ orderMut };

            auto order = std::make_unqiue<Order>(incoming);
            auto& book = (order->side == Side::Buy) ? bids_ : asks_;
    
            auto it = book.find(order->price);
            if (it == book.end()) {
                auto limit = std::make_unqiue<Limit>();
                limit->price = order->price;
                limit->append(order.get());
                it = book.emplace(order->price, std::move(limit)).first;
            } else {
                it->second->append(order.get())
            }

            order_index_[order->id] = order.get();
            order_owner_.push_back(std::move(order));

        }

        void cancelOrder(size_t orderID) {

            std::scoped_lock ordersLock{ orderMut };
            
            auto it = order_index_.find(orderID);
            if (it == order_index_.end()) {
                orderCancelReject();
                return;
            }

            Order* order = it->second;

            if (order->dBit == 1) {
                orderCancelReject();
                return;
            }

            auto& book = (order->side == Side::Buy) ? bids_ : asks_;
            auto limitIt = book.find(order->price);
            if ( limitIt != book.end()) {
                limitIt->second->remove(order);
                if ( limitIt->second->empty() ) {
                    book.erase(limit);
                }
            }


            order->dBit = 1;
            order_index_.erase(it);
            sendConf();
        }

        std::optional<Limit*> bestBid() {
            if ( bids_.empty() ) {
                return std::nullopt;
            } else {
                return bids_.begin()->second.get();
            }
        }

        std::optional<Limit*> bestAsk() {
            if (asks_.empty() ) {
                return std::nullopt;
            } else {
                return asks_.begin()->second.get();
            }
        }

        std::optional<int64_t> GetSpread() {
            if ( ask_.empty() || bids_.empty() ) {
                return std::nullopt;
            } else {
                spread = (bids_.begin()->second.get() - asks_.begin()->second.get());
                return static_cast<int64_t>(spread);
            }
        }

        Order* ModifyOrder(const Order *newOrder) {

            auto it = order_index_.find(newOrder->orderID);

            if ( it == order_index_.end()) {
                std::cerr << "Modify OrderID not found";
                return  nullptr;
            }

            cancelOrder(newOrder->orderId);
            return appendOrder(newOrder);
        }

        void printBook() const {
            
            std::cout << "--------Asks------\n";
            for ( const auto& [ Price, Limit ] : asks_ ) {
                printLevel(*Limit);
            }

            std::cout << "-------Bids--------\n";
            for ( const auto& [ Price, Limit ] : bids_ ) {
                printLevel(*Limit);
            }
        }

        void printLevel(const Limit& lmt) const {

            size_t orderCount{};

            for (Order* o = lmt.head; o; o = o->next) ++orderCount;


            std::cout << std::format(" {} | Vol: {} | Orders: {} ", lmt.Price, lmt.total_volume, orderCount) << "\n";
        }

        std::size_t orderBookSize() {
            {
                std::scoped_lock<std::mutex> lock(orderMut);

                return order_index_.size();
            }
        }

        









            



            

        



        
      

    private:


        std::unordered_map<Price, std::unique_ptr<Limit>, std::greater<Price>> bids_; // prices in which buyer wants to buy
        std::unordered_map<Price, std::unique_ptr<Limit> asks_; // prices a buyer willing to sell

        std::unordered_map<OrderID, Order*> order_index_; //This structure handles fast indexing and lookups.
        std::vector<std::unique_ptr<Order>> order_owner_; //This structure manages the lifetime and storage of all orders.

        std::mutex obMut; // Mutex for order book operations

        OrderBook() {
            std::cout << "Initialized\n";
        }

        ~OrderBook() = default;
};


