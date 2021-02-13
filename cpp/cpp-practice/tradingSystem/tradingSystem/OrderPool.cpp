//
//  OrderPool.cpp
//  tradingSystem
//
//  Created by Yue Zhang on 13/2/21.
//

#include "OrderPool.hpp"

void OrderPoolBuilder::init_orders(int num, double price_low, double price_high, int quantity_high, bool verbose) {
    srand((int)time(nullptr));
    // test data
    int id = 1;
    while (num > 0) {
        Order ord;
        string ord_id = "order_" + to_string(id);
        double price = (price_high - price_low) * rand()/RAND_MAX + price_low;
        int quantity = rand() % quantity_high + 1;
        long order_time = time(0);
        int direction = rand() % 2;
        
        ord.order_id = ord_id;
        ord.price = price;
        ord.quantity = quantity;
        ord.time = order_time;
        ord.direction = direction;
        
        if (verbose) {
            printf("orderID: %s, price: %f, quantity: %d, time: %ld, direction: %d",
                   ord.order_id.c_str(), ord.price, ord.quantity, ord.time, ord.direction);
            cout << endl;
        }
        
        if (direction == 1) buy_q.push(ord);
        else sell_q.push(ord);
        id++;
        num--;
    }

}

void OrderPoolBuilder::add_buy_order(Order ord) {
    if (ord.direction != 1) return;
    if (buy_q.empty()) {
        buy_q.push(ord);
        return;
    }
    if (ord.price < sell_q.top().price) return;
    buy_q.push(ord);

}

void OrderPoolBuilder::add_sell_order(Order ord) {
    if (ord.direction != 0) return;
    if (sell_q.empty()) {
        sell_q.push(ord);
        return;
    }
    if (ord.price > buy_q.top().price) return;
    sell_q.push(ord);
}

void OrderPoolBuilder::run_matching(int mod, bool verbose){
    int n_orders = 0;
    while (!buy_q.empty()) {
        Order buy_ord = buy_q.top();
        buy_q.pop();

        string order_id = buy_ord.order_id;
        int buy_quantity = buy_ord.quantity;
        double buy_price = buy_ord.price;
        int total_quantity = buy_quantity;
        double total_cost = 0.0;

        while (!sell_q.empty()) {
            Order sell_ord = sell_q.top();
            sell_q.pop();
            if (sell_ord.price <= buy_price) {
                n_orders++;
                if (verbose && n_orders % mod == 0) {
                    printf("%d number of orders filled", n_orders);
                    cout << endl;
                }
                if (buy_quantity > sell_ord.quantity) {
                    // buy order partially filled
                    total_cost += sell_ord.quantity * sell_ord.price;
                    buy_quantity -= sell_ord.quantity;
                    buy_ord.quantity = buy_quantity - sell_ord.quantity;
                    // push back unfilled buy order
                    buy_q.push(buy_ord);
                } else if (buy_quantity == sell_ord.quantity) {
                    // buy and sell order fully filled
                    total_cost += sell_ord.quantity * sell_ord.price;
                    buy_quantity = 0;
                } else {
                    // sell order partially fulfilled
                    total_cost += buy_quantity * sell_ord.price;
                    buy_quantity = 0;
                    sell_ord.quantity = sell_ord.quantity - buy_quantity;
                    // push back unfilled sell order
                    sell_q.push(sell_ord);
                }
            } else {
                if (verbose) cout << "[error] minimum sell price is larger than maximum buy price" << endl;
                break;
            }

            if (buy_quantity == 0) {
                double buy_avg_price = total_cost/total_quantity;
                if (verbose) {
                    cout << "order id: " << order_id << " filled with price: " << buy_avg_price << endl;
                }
                break;
            };
        }
    }
}

