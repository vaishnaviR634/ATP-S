#include "Orderbookv3.hpp"
#include <fstream>
#include <iostream>

 // comparator function for bids priority queue
class fifoBuyComparator{
    public:
    bool operator()(buyOrder A, buyOrder B){
    if(A.price != B.price){
        //greater price, greater priority
        if(A.price < B.price){ 
            return true;
        }
        else{
            return false;
        }
    }
    else{
        if(A.timeNumber > B.timeNumber){
             //earlier, more priority
            return true;
        }
        else{
            return false;
        }
    }
}    
};

 // comparator funciton for asks priority queue
class fifoSellComparator{
    public:
    bool operator()(sellOrder A, sellOrder B){
    if(A.price != B.price){
        if(A.price > B.price){ 
            // lesser price, higher priority
            return true;
        }
        else{
            return false;
        }
    }
    else{
        if(A.timeNumber > B.timeNumber){
             // earlier, higher priority
            return true; 
        }
        else{
            return false;
        }
    }
}
};




 // calling this function runs the Fifo matching algorithm. This function is "automatically" run when a buy order/sell order is placed
void matchFifo(){
    clear();
    showOrderBook();
     // so you wont have some access error
    if(bids.empty() || asks.empty()){return;}
    // The below Matching Code is for FIFO
    if(bids.top().price == asks.top().price && bids.size() != 0 && asks.size() != 0){
         // checks if any order can happen, i.e. if the price of the best buy order and best sell order match
        int64_t quantityMatched = 0;
        int64_t priceMatched = asks.top().price;
        if(asks.top().orderQuantity > bids.top().orderQuantity){
            while(asks.top().orderQuantity > bids.top().orderQuantity && asks.top().price == bids.top().price){
                auto temp = asks.top();
                temp.orderQuantity -= bids.top().orderQuantity; quantityMatched += bids.top().orderQuantity;
                asks.pop();
                bids.pop();
                asks.push(temp);
                if(bids.empty() || asks.empty()){break;}
            }
            if(bids.size() != 0 && asks.size() != 0){
                if(bids.top().orderQuantity > asks.top().orderQuantity && asks.top().price == bids.top().price){
                    auto temp = bids.top();
                    temp.orderQuantity -= asks.top().orderQuantity; quantityMatched += asks.top().orderQuantity;
                    asks.pop();
                    bids.pop();
                    bids.push(temp);
                }
                else if(asks.top().price == bids.top().price){
                    quantityMatched += bids.top().orderQuantity;
                    bids.pop();
                    asks.pop();
                }
            }
        }
        else if(bids.top().orderQuantity > asks.top().orderQuantity){
            while(bids.top().orderQuantity > asks.top().orderQuantity){
                auto temp = bids.top();
                temp.orderQuantity -= asks.top().orderQuantity; quantityMatched += asks.top().orderQuantity;
                asks.pop();
                bids.pop();
                bids.push(temp);
                if(bids.size() == 0 || asks.size() == 0){break;}
            }
            if(bids.size() != 0 && asks.size() != 0){
                if(bids.top().orderQuantity < asks.top().orderQuantity && asks.top().price == bids.top().price){
                    auto temp = asks.top();
                    temp.orderQuantity -= bids.top().orderQuantity; quantityMatched += bids.top().orderQuantity;
                    asks.pop();
                    bids.pop();
                    asks.push(temp);
                }
                else if(asks.top().price == bids.top().price){
                    quantityMatched += bids.top().orderQuantity;
                    bids.pop();
                    asks.pop();
                }
            }
        }
        else{
            quantityMatched = asks.top().orderQuantity;
            asks.pop();
            bids.pop();
        }
        
        matchedOrders.push_back({priceMatched, quantityMatched});
        // // cout<<"Trade Ahoy! : "<<asks.top().price<<endl;                                                                                
        // matchedOrders.push_back(bids.top().price); // then the order is registered to the list of matched orders
        // bids.pop();// the bid price and ask price at which the trade happened, 
        // asks.pop();// are popped form their priority queues  
    }
    clear();
    showOrderBook();
}




// int64_t main(){
// //--------------------
// // freopen("input.txt","r",stdin);
// freopen("output.txt","w",stdout);
// //-------------------
// // for (int64_t i = 1; i<10; i++){
// //     makeBuyOrder(i*i, i);
// // }
// // makeSellOrder(300,1);
// // makeSellOrder(100,20);
// // // makeSellOrder(81, 9);
// // // makeBuyOrder(100, 10);
// // makeSellOrder(81, 2);
// // makeBuyOrder(100,11);
// // makeBuyOrder(100, 10);
// // makeSellOrder(100, 12);

// // makeBuyOrder(100,30);
// // makeSellOrder(100,10);


// // showOrderBook();

// while(true){
//     string s; cin>>s;
//     if(s == "stop"){
//        clear();
//        cout<<"Stopped"; 
//        break;
//     }
//     else{
//         if(tolower(s[0]) == 'b'){
//             parse(s);
//         }
//         else if(tolower(s[0]) == 's'){
//             parse(s);
//         }
//         else{
//             clear();
//             cout<<"Invalid input, try again\n";
//         }
//     }
// }
// return 0;
// }
