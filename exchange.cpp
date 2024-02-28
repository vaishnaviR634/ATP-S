#include <cstdint>
#include <memory>
#include <sstream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <chrono>
#include <queue>

struct order {
  int64_t order_id;
	long double price;
	int64_t quantity;
    int64_t time;
	bool buy; //0=buy,1=sell
	bool matched;
    // order(int64_t ID, long double price, int64_t quantity,bool buy  ){
    //     this->buy=buy; this->price=price;this->quantity=quantity;
    //     this->matched=0; this->time=//add time;
    // }
    order();
       bool operator<( order &other) {
        if (price != other.price) {
            return (price < other.price) ^ buy;
        } else {
            return time < other.time;
        }
    }
};


order parse(std::shared_ptr<char[]> buffer) {
	std::istringstream stream(buffer.get());
	order ord;
	stream >> ord.order_id >> ord.buy >> ord.price >> ord.quantity;
	ord.matched = false;
     ord.time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	return ord;
}

int64_t ask_price,ask_quantity,bid_price,bid_quantity;
//ask price-> highest seller acceptence price
std::priority_queue<order> BUY,SELL;

void add_to_priority_queue(order ord) {
	 //take the order, add it to the necessary priority queue
     bool display=false;
       if (ord.buy) {
              BUY.push(ord);
            if (ord.price >= ask_price) {
                match();
                return;
            }
          //  if (ord.price >= bid_price) display = true;

        } else {
              SELL.push(ord);
            if (ord.price <= bid_price) {
                match();
                return;
            }
           // if (ord.price <= ask_price) display = true;
        }

}

void match() {
	// match the orders

while(BUY.size()>0 && SELL.size()>0){

 order& buy_top = const_cast<order&>(BUY.top());
        order& sell_top = const_cast<order&>(SELL.top());

          if(buy_top.price < sell_top.price){break;}

        if(buy_top.quantity > sell_top.quantity){
          
             std::cout<<"Order No: "<<buy_top.order_id<<std::endl;
             std::cout<<"Partial Buy Order executed! Quantity Bought: "<<sell_top.quantity;
                std::cout<<"Order No: "<<sell_top.order_id<<std::endl;
             std::cout<<"Sell Order executed!";

                SELL.pop();
            buy_top.quantity-=sell_top.quantity;
        }
        else if(buy_top.quantity < sell_top.quantity){
            
             std::cout<<"Order No: "<<sell_top.order_id<<std::endl;
             std::cout<<"Partial Sell Order executed! Quantity Sold: "<<buy_top.quantity;
                std::cout<<"Order No: "<<buy_top.order_id<<std::endl;
             std::cout<<"Buy Order executed!";
            BUY.pop();
            sell_top.quantity-=buy_top.quantity;
        }
        else {  
            
             std::cout<<"Order No: "<<buy_top.order_id<<std::endl;
             std::cout<<" Buy Order executed!";
                std::cout<<"Order No: "<<sell_top.order_id<<std::endl;
             std::cout<<"Sell Order executed!";
            BUY.pop(); SELL.pop();

        }


}


}
