#ifndef MARKET_DATA_HPP
#define MARKET_DATA_HPP

#include <string>
#include "nlohmann/json.hpp"

std::string sendRequest(const std::string& url, const nlohmann::json& payload, const std::string& accessToken = "");
std::string getAccessToken(const std::string& clientId, const std::string& clientSecret);
std::vector<std::string> getInstruments(const std::string& accessToken);
void getCurrentPositions(const std::string& accessToken);
void getOpenOrdersByCurrency(const std::string &currency,const std::string &accessToken);
void getUserTradesByCurrency(const std::string &currency, const std::string &accessToken);
#endif 
