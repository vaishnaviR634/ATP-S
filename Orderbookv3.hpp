//===-- ATP-SERVER/Orderbookv3.hpp --===//
//
// Part of the ATP-SERVER Project
//===-----------------------------===//
///
/// \file
/// This file contains the declarations
/// of the buyOrder and sellOrder class
/// and the priority queues bids,asks and 
/// vector matchedOrder
/// \brief
/// Follows a FIFO exchang
///Takes string as input and parses it into order format
/// Stops when input string is 'stop'
//===-----------------------------===//

#include <algorithm>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <iomanip>
#include <fstream>
#include <chrono> 
using namespace std;
#define int long long

/// global scope typedef for maintaining timepoints
typedef std::chrono::time_point<std::chrono::system_clock> t_point;

/// Sets global timestamp to 0;
void Start_Clock();

/// Returns the current time since the clock was started.
t_point Get_Time();


template <typename T> void disp(vector<T> v){
    for(auto thing : v){
        cout<<thing<<" ";
    }
    cout<<endl;
}

template <typename T, typename U> void disp(vector<pair<T,U>> v){
    for(auto thing : v){
        cout<<thing.first<<" "<<thing.second<<endl;
    }
    cout<<endl;
}

// output.txt is the output file where all the orders and matches are written
void clear(){
    freopen("output.txt", "w",stdout); 
}


// time (increases by 1 after every bid or ask)
// int64_t timeNumber = 0; 


// buyOrder class: Object of this class represents a buy order
class buyOrder{ 
    public:
    // string orderID;  
    int64_t64_t orderQuantity; 
    int64_t price;
    int64_t timeNumber;
};

 // sellOrder class: Object of this class represents a sell order
class sellOrder{
    public: 
    // string orderID;
    int64_t orderQuantity;
    int64_t price;
    int64_t timeNumber;
};

priority_queue<buyOrder, vector<buyOrder>, fifoBuyComparator> bids; 

priority_queue<sellOrder, vector<sellOrder>, fifoSellComparator> asks; 

vector<pair<int64_t,int64_t>> matchedOrders; 


void showpq(priority_queue<buyOrder, vector<buyOrder>, fifoBuyComparator> p){
    while(p.size()){
        cout<< p.top().price <<" "<<p.top().orderQuantity<<" "<<p.top().timeNumber<<endl;
        p.pop();
    }
    
}

void showpq(priority_queue<sellOrder, vector<sellOrder>, fifoSellComparator> p){
    while(p.size()){
        cout<<p.top().price<<" "<<p.top().orderQuantity<<" "<<p.top().timeNumber<<endl;
        p.pop();
    }
}



void showOrderBook(){
    cout<<"Bids :"<<endl;
    if(bids.size()){showpq(bids);}
    cout<<endl;
    cout<<"Asks :"<<endl;
    if(asks.size()){showpq(asks);}
    cout<<endl;
    cout<<"Number of Matched Orders: "<<matchedOrders.size()<<endl;
    cout<<"Matched Order Prices & Quantities: "<<endl;
    if(matchedOrders.size()){disp(matchedOrders);}
}

 // function that `places` a buy order. It pushes the buy order object int64_to the bids priority queue
void makeBuyOrder(int64_t buyingprice, int64_t orderquantity){
    buyOrder orderA; 
    orderA.price = buyingprice; 
    orderA.timeNumber = t_point; 
    orderA.orderQuantity = orderquantity;
    bids.push(orderA);
    matchFifo();
}

void makeSellOrder(int64_t sellingprice, int64_t orderquantity){ 
    sellOrder orderA;
    orderA.price = sellingprice;
    orderA.timeNumber = t_point
    orderA.orderQuantity = orderquantity;
    asks.push(orderA);
    matchFifo();
}

// parses the input from user, which is a string
void parse(string s){ 
     // string number
    string stNum = "";
    string stQuantity = "";
    int64_t n = s.length(); int64_t indx = -1;
    for(int64_t i = 1; i<n; i++){
        if(tolower(s[i]) != 'q'){
            stNum.push_back(s[i]);
        }
        else{
            indx = i; break;
        }
    }
    double price = atof(stNum.c_str());
    if(indx == -1){
        stQuantity = "1";
    }
    else{
        for(int64_t i = indx+1; i<n; i++){
            stQuantity.push_back(s[i]);
        }
    }
    int64_t quantity = atoi(stQuantity.c_str());
    if(tolower(s[0]) == 'b'){
        makeBuyOrder(price, quantity);
    }
    else if(tolower(s[0]) == 's'){
        makeSellOrder(price, quantity);
    }
}

