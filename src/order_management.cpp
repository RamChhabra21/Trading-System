#include <iostream>
#include "order_management.hpp"
#include "market_data.hpp"

void placeOrder(const std::string &price, const std::string &accessToken, const std::string &amount, const std::string &instrument, const std::string &type)
{
    nlohmann::json payload = {
        {"jsonrpc", "2.0"},
        {"method", "private/buy"},
        {"params", {{"instrument_name", instrument}, {"type", type}, {"price", price}, {"amount", amount}}},
        {"id", 1}};

    std::string response = sendRequest("https://test.deribit.com/api/v2/private/buy", payload, accessToken);
    std::cout << "Place Order Response: " << response << std::endl;
}

void cancelOrder(const std::string &accessToken, const std::string &orderID)
{
    nlohmann::json payload = {
        {"jsonrpc", "2.0"},
        {"method", "private/cancel"},
        {"params", {{"order_id", orderID}}},
        {"id", 6}};

    std::string response = sendRequest("https://test.deribit.com/api/v2/private/cancel", payload, accessToken);
    std::cout << "Cancel Order Response: " << response << std::endl;
}

void modifyOrder(const std::string &accessToken, const std::string &orderID, int amount, double price)
{
    nlohmann::json payload = {
        {"jsonrpc", "2.0"},
        {"method", "private/edit"},
        {"params", {{"order_id", orderID}, {"amount", amount}, {"price", price}}},
        {"id", 11}};

    std::string response = sendRequest("https://test.deribit.com/api/v2/private/edit", payload, accessToken);
    std::cout << "Modify Order Response: " << response << std::endl;
}

void getOrderBook(const std::string &accessToken, const std::string &instrument)
{
    nlohmann::json payload = {
        {"jsonrpc", "2.0"},
        {"method", "public/get_order_book"},
        {"params", {{"instrument_name", instrument}}},
        {"id", 15}};

    std::string response = sendRequest("https://test.deribit.com/api/v2/public/get_order_book", payload, accessToken);
    auto responseJson = nlohmann::json::parse(response);

    if (responseJson.contains("result"))
    {
        std::cout << "\nOrder Book for " << instrument << ":\n";

        // Spread
        double best_bid_price = responseJson["result"]["best_bid_price"];
        double best_ask_price = responseJson["result"]["best_ask_price"];
        double spread = best_ask_price - best_bid_price;

        // Print Best Bid and Ask
        std::cout << "Best Bid Price: " << best_bid_price << ", Amount: " << responseJson["result"]["best_bid_amount"] << '\n';
        std::cout << "Best Ask Price: " << best_ask_price << ", Amount: " << responseJson["result"]["best_ask_amount"] << '\n';
        std::cout << "Spread: " << spread << '\n';

        // Full Bid and Ask Depth
        std::cout << "\nBid Depth:\n";
        for (const auto &bid : responseJson["result"]["bids"])
        {
            std::cout << "Price: " << bid[0] << ", Amount: " << bid[1] << '\n';
        }

        std::cout << "\nAsk Depth:\n";
        for (const auto &ask : responseJson["result"]["asks"])
        {
            std::cout << "Price: " << ask[0] << ", Amount: " << ask[1] << '\n';
        }

        // Additional Summaries
        double total_bid_volume = 0;
        double total_ask_volume = 0;

        for (const auto &bid : responseJson["result"]["bids"])
        {
            total_bid_volume = total_bid_volume + static_cast<double>(bid[1]);
        }
        for (const auto &ask : responseJson["result"]["asks"])
        {
            total_ask_volume = total_ask_volume + static_cast<double>(ask[1]);
        }

        std::cout << "\nSummary:\n";
        std::cout << "Total Bid Volume: " << total_bid_volume << '\n';
        std::cout << "Total Ask Volume: " << total_ask_volume << '\n';
        std::cout << "Market Midpoint: " << (best_bid_price + best_ask_price) / 2 << '\n';
    }
    else
    {
        std::cerr << "Error fetching order book: " << responseJson["error"]["message"] << std::endl;
    }
}
