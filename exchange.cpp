#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include "Parser.cpp"
/*
FIFO Exchange

Order Format:
0 X P Q
1 X Q

0 -> Limit Order
1 -> Market Order
X = 0/1
0 -> Sell
1 -> Buy
P -> Price
Q -> Quantity


Broadcast Format:
T AP AQ BP BQ FP FQ

T -> Time in milliseconds from the start of the exchange
AP -> Ask Price
AQ -> Ask Quantity
BP -> Bid Price
BQ -> Bid Quantity
FP -> Fill Price
FQ -> Fill Quantity
(If no fill, FP and FQ are not present)
(If multiple fills, multiple lines are printed)

If market order is not filled, print "Market order quantity is more than available quantity" and trade with available quantity

Input -1 to end the exchange
*/

long long time_of_start_of_exchange = 0;

using namespace std::chrono;

void Start_Exchange() {
    time_of_start_of_exchange = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

long long Get_Time() {
    auto time = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
    return (time - time_of_start_of_exchange);
}

struct Limit_Order {
    bool buy;  // true -> buy, false -> sell
    long long price, quantity, time;
    Limit_Order(bool buy, long long price, long long quantity, long long time) {
        this->buy = buy;
        this->price = price;
        this->quantity = quantity;
        this->time = time;
    }

    // FIFO:
    bool operator<(const Limit_Order &other) const {
        // if (buy) {
        //     if (price != other.price)
        //         return price > other.price;
        //     else
        //         return time < other.time;
        // } else {
        //     if (price != other.price)
        //         return price < other.price;
        //     else
        //         return time < other.time;
        // }
        // changed here
        if (price != other.price) {
            return (price < other.price) ^ buy;
        } else {
            return time < other.time;
        }
    }

    /*
    Pro-rata:
    bool operator < (const Limit_Order &other) const{
        if(price != other.price){
            if(buy) return price > other.price;
            else return price < other.price;
        }
        if (quantity != other.quantity) return quantity < other.quantity;
        return time < other.time;
    }
    */
};

class Exchange {
   private:
    std::set<Limit_Order> buy_orders, sell_orders;
    long long ask_price, ask_quantity, bid_price, bid_quantity, total_quantity_ask, total_quantity_bid;
    std::map<long long, long long> ask_prices, bid_prices;

    void Update_Market_Values() {
        if (buy_orders.empty()) {
            bid_price = -1;
            bid_quantity = 0;
            total_quantity_bid = 0;
        } else {
            bid_price = buy_orders.begin()->price;
            bid_quantity = bid_prices[bid_price];
        }

        if (sell_orders.empty()) {
            ask_price = 1e15;
            ask_quantity = 0;
            total_quantity_ask = 0;
        } else {
            ask_price = sell_orders.begin()->price;
            ask_quantity = ask_prices[ask_price];
        }
    }

    void Broadcast(long long fill_price, long long fill_quantity) {
        std::cout << Get_Time() << "\t" << ask_price << "\t" << ask_quantity << "\t" << bid_price << "\t" << bid_quantity << "\t" << fill_price << "\t" << fill_quantity << "\n";
    }

    void Broadcast() {
        std::cout << Get_Time() << "\t" << ask_price << "\t" << ask_quantity << "\t" << bid_price << "\t" << bid_quantity << "\n";
    }

   public:
    void Fill_Market_Order(bool buy, long long quantity) {
        long long fill_price = -1, fill_quantity = 0, temp_quantity, current_quantity, temp_time;
        if (buy) {
            if (quantity > total_quantity_ask) {
                quantity = total_quantity_ask;
                std::cout << "Market order quantity is more than available quantity\n";
            }

            while (!sell_orders.empty() && quantity > 0) {
                auto it = sell_orders.begin();
                fill_price = it->price;
                current_quantity = it->quantity;
                temp_time = it->time;
                temp_quantity = std::min(current_quantity, quantity);
                quantity -= temp_quantity;
                total_quantity_ask -= temp_quantity;
                current_quantity -= temp_quantity;
                fill_quantity = temp_quantity;
                if (current_quantity == 0) {
                    sell_orders.erase(it);
                    ask_prices[fill_price] -= temp_quantity;
                    Update_Market_Values();
                    if (ask_prices[fill_price] == 0) {
                        ask_prices.erase(fill_price);
                        Broadcast(fill_price, fill_quantity);
                        fill_quantity = 0;
                    }
                } else {
                    sell_orders.erase(it);
                    sell_orders.insert(Limit_Order(false, fill_price, current_quantity, temp_time));
                    ask_prices[fill_price] -= temp_quantity;
                    Update_Market_Values();
                }
            }
            if (fill_quantity > 0) Broadcast(fill_price, fill_quantity);
        } else {
            if (quantity > total_quantity_bid) {
                quantity = total_quantity_bid;
                std::cout << "Market order quantity is more than available quantity\n";
            }

            while (!buy_orders.empty() && quantity > 0) {
                auto it = buy_orders.begin();
                fill_price = it->price;
                current_quantity = it->quantity;
                temp_time = it->time;
                temp_quantity = std::min(current_quantity, quantity);
                quantity -= temp_quantity;
                total_quantity_bid -= temp_quantity;
                current_quantity -= temp_quantity;
                fill_quantity = temp_quantity;
                if (current_quantity == 0) {
                    buy_orders.erase(it);
                    bid_prices[fill_price] -= temp_quantity;
                    Update_Market_Values();
                    if (bid_prices[fill_price] == 0) {
                        bid_prices.erase(fill_price);
                        Broadcast(fill_price, fill_quantity);
                        fill_quantity = 0;
                    }
                } else {
                    buy_orders.erase(it);
                    buy_orders.insert(Limit_Order(true, fill_price, current_quantity, temp_time));
                    bid_prices[fill_price] -= temp_quantity;
                    Update_Market_Values();
                }
            }
            if (fill_quantity > 0) Broadcast(fill_price, fill_quantity);
        }
    }

    void Add_Limit_Order(bool buy, long long price, long long quantity, long long time) {
        bool broadcast = false;
        if (buy) {
            if (price >= ask_price) {
                Match(buy, price, quantity, time);
                return;
            }
            buy_orders.insert(Limit_Order(buy, price, quantity, time));
            bid_prices[price] += quantity;
            total_quantity_bid += quantity;
            if (price >= bid_price) broadcast = true;
        } else {
            if (price <= bid_price) {
                Match(buy, price, quantity, time);
                return;
            }
            sell_orders.insert(Limit_Order(buy, price, quantity, time));
            ask_prices[price] += quantity;
            total_quantity_ask += quantity;
            if (price <= ask_price) broadcast = true;
        }
        if (broadcast) {
            Update_Market_Values();
            Broadcast();
        }
    }

    void Match(bool buy, long long price, long long quantity, long long time) {
        long long fill_price = -1, fill_quantity = 0, temp_quantity, current_quantity, temp_time;

        if (buy) {
            while (!sell_orders.empty() && quantity > 0) {
                auto it = sell_orders.begin();
                if (it->price > price) break;
                fill_price = it->price;
                temp_time = it->time;
                current_quantity = it->quantity;
                temp_quantity = std::min(current_quantity, quantity);
                quantity -= temp_quantity;
                total_quantity_ask -= temp_quantity;
                current_quantity -= temp_quantity;
                fill_quantity += temp_quantity;
                if (current_quantity == 0) {
                    sell_orders.erase(it);
                    ask_prices[fill_price] -= fill_quantity;
                    Update_Market_Values();
                    if (ask_prices[fill_price] == 0) {
                        ask_prices.erase(fill_price);
                        Broadcast(fill_price, fill_quantity);
                        fill_quantity = 0;
                    }
                } else {
                    sell_orders.erase(it);
                    sell_orders.insert(Limit_Order(false, fill_price, current_quantity, temp_time));
                    ask_prices[fill_price] -= fill_quantity;
                    Update_Market_Values();
                }
            }
            if (quantity > 0)
                Add_Limit_Order(buy, price, quantity, time);
            else if (fill_quantity > 0)
                Broadcast(fill_price, fill_quantity);
        } else {
            while (!buy_orders.empty() && quantity > 0) {
                auto it = buy_orders.begin();
                if (it->price < price) break;
                fill_price = it->price;
                temp_time = it->time;
                current_quantity = it->quantity;
                temp_quantity = std::min(current_quantity, quantity);
                quantity -= temp_quantity;
                total_quantity_bid -= temp_quantity;
                current_quantity -= temp_quantity;
                fill_quantity += temp_quantity;
                if (current_quantity == 0) {
                    buy_orders.erase(it);
                    bid_prices[fill_price] -= fill_quantity;
                    Update_Market_Values();
                    if (bid_prices[fill_price] == 0) {
                        bid_prices.erase(fill_price);
                        Broadcast(fill_price, fill_quantity);
                        fill_quantity = 0;
                    }
                } else {
                    buy_orders.erase(it);
                    buy_orders.insert(Limit_Order(true, fill_price, current_quantity, temp_time));
                    bid_prices[fill_price] -= fill_quantity;
                    Update_Market_Values();
                }
            }
            if (quantity > 0)
                Add_Limit_Order(buy, price, quantity, time);
            else if (fill_quantity > 0)
                Broadcast(fill_price, fill_quantity);
        }
    }

    Exchange() {
        ask_price = 1e15;
        bid_price = -1;
        total_quantity_ask = total_quantity_bid = 0;
        ask_quantity = bid_quantity = 0;
        ask_prices.clear();
        bid_prices.clear();
        buy_orders.clear();
        sell_orders.clear();
        // std::ifstream fin("Pending_Orders.txt");
        // while(fin){
        //     bool buy;
        //     long long price, quantity, time;
        //     fin >> buy >> price >> quantity >> time;
        //     if(fin) Add_Limit_Order(buy, price, quantity, time);
        // }
        // fin.close();
    }

    // ~Exchange(){
    //     std::cout << "Exchange is Closing; All Pending Orders are being saved to Pending_Orders.txt" << '\n';
    //     std::cout << "Last Broadcasted Values: " << bid_price << " " << bid_quantity << " " << ask_price << " " << ask_quantity << '\n';
    //     std::ofstream fout("Pending_Orders.txt");
    //     for(auto it:buy_orders) fout << it.buy << "\t" << it.price << "\t" << it.quantity << "\t" << it.time << '\n';
    //     for(auto it:sell_orders) fout << it.buy << "\t" << it.price << "\t" << it.quantity << "\t" << it.time << '\n';
    //     fout.close();
    //     buy_orders.clear();
    //     sell_orders.clear();
    //     ask_prices.clear();
    //     bid_prices.clear();
    // }
};

int main() {
    Start_Exchange();
    Exchange exchange;
    while (true) {
        // short int order_type;
        // std::cin >> order_type;
        // // bit-1 order type
        // // 
        // if (order_type == -1) break;
        // bool buy;
        // long long price, quantity;
        // if (!order_type) {
        //     std::cin >> buy >> price >> quantity;
        //     exchange.Add_Limit_Order(buy, price, quantity, Get_Time());
        // } else {
        //     std::cin >> buy >> quantity;
        //     exchange.Fill_Market_Order(buy, quantity);
        // }
          // Read the order from the network buffer
        std::string order;
        std::getline(std::cin, order);
        
        // Check for termination signal
        if (order == "-1") break;

        // Call the parser function to process the order
        parse_order(order);
    }
    return 0;
}
