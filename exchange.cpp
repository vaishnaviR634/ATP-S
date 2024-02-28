#include <cstdint>
#include <memory>
#include <sstream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <chrono>
#include <queue>

struct order {
    int64_t order_id, quantity, time;
	long double price;
	bool buy; //0=buy,1=sell
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
     ord.time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	return ord;
}

std::priority_queue<order> BUY,SELL;

void add_to_priority_queue(const order& ord) {
    if ((ord.buy && ord.price >= SELL.top().price) || (!ord.buy && ord.price <= BUY.top().price)) {
        if (ord.buy) {
            BUY.push(ord);
        } else {
            SELL.push(ord);
        }
        match();
    }
}

order& buy_top = const_cast<order&>(BUY.top());
 order& sell_top = const_cast<order&>(SELL.top());

void match() {
while(BUY.size()>0 && SELL.size()>0){

 buy_top = const_cast<order&>(BUY.top());
  sell_top = const_cast<order&>(SELL.top());

    if(buy_top.price < sell_top.price){break;}

        if(buy_top.quantity > sell_top.quantity){
              display(0,sell_top.quantity);
                SELL.pop();
            buy_top.quantity-=sell_top.quantity;
        }
        else if(buy_top.quantity < sell_top.quantity){
                display(1,buy_top.quantity);
            BUY.pop();
            sell_top.quantity-=buy_top.quantity;
        }
        else {  
            display(2,0);
            BUY.pop(); SELL.pop();

        }
    
    
}

}

void display(int x, long long q){
      std::cout<<"Order ID: "<<buy_top.order_id<<std::endl;
              if(x==0)std::cout<<"Partial ";
             std::cout<<" Buy Order executed!\n"; 
             long long q1=buy_top.quantity;
             if(x==0) q1=q;
             std::cout<<" Quantity Bought: "<<q<<" at Price:"<<sell_top.price;

              std::cout<<"Order ID: "<<sell_top.order_id<<std::endl;
                  if(x==1)std::cout<<"Partial ";
             std::cout<<"Sell Order executed!\n";
             long long q2= sell_top.quantity;
             if(x==1)q2=q;
             std::cout<<" Quantity Sold: "<<q<<" at Price:"<<sell_top.price;
}
