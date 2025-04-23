#ifndef ORDER_MANAGEMENT_HPP
#define ORDER_MANAGEMENT_HPP

#include <string>

void placeOrder(const std::string& price, const std::string& accessToken, const std::string& amount, const std::string& instrument, const std::string &type);
void cancelOrder(const std::string& accessToken, const std::string& orderID);
void modifyOrder(const std::string& accessToken, const std::string& orderID, int amount, double price);
void getOrderBook(const std::string& accessToken, const std::string& instrument);

#endif 
