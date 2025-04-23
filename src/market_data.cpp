#include <iostream>
#include <string>
#include "market_data.hpp"
#include "nlohmann/json.hpp"
#include <curl/curl.h>
#include "credentials.hpp"

using json = nlohmann::json;

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

std::string sendRequest(const std::string &url, const json &payload, const std::string &accessToken)
{
    std::string readBuffer;
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L); // Set the HTTP method to POST

        std::string jsonStr = payload.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        if (!accessToken.empty())
        {
            headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return readBuffer;
}

std::string getAccessToken(const std::string &clientId, const std::string &clientSecret)
{
    std::string url = "https://test.deribit.com/api/v2/public/auth";
    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "public/auth"},
        {"params", {{"client_id", clientId}, {"client_secret", clientSecret}, {"grant_type", "client_credentials"}, {"scope", "session:read"}}},
        {"id", 1}};

    std::string response = sendRequest(url, payload);
    auto responseJson = json::parse(response);

    if (responseJson.contains("result"))
    {
        return responseJson["result"]["access_token"];
    }
    else
    {
        std::cerr << "Error fetching access token: " << responseJson["error"]["message"] << std::endl;
        return "";
    }
}

// Function to get a list of available instruments
std::vector<std::string> getInstruments(const std::string &accessToken)
{
    std::vector<std::string> instruments;
    std::string url = "https://test.deribit.com/api/v2/public/get_instruments";

    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "public/get_instruments"},
        {"params", {{"currency", "BTC"}}},
        {"id", 1}};

    std::string response = sendRequest(url, payload, accessToken);
    auto responseJson = json::parse(response);

    if (responseJson.contains("result"))
    {
        for (const auto &item : responseJson["result"])
        {
            instruments.push_back(item["instrument_name"]);
        }
    }
    else
    {
        std::cerr << "Error fetching instruments: " << responseJson["error"]["message"] << std::endl;
    }

    return instruments;
}

void getCurrentPositions(const std::string &accessToken)
{
    std::vector<std::string> positions;

    // UPDATED: URL now includes query parameters
    std::string url = "https://test.deribit.com/api/v2/private/get_positions?";

    // Initialize CURL and set up options
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();

    if (curl)
    {
        // Set URL for the API call
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Add Authorization header with Bearer token
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // String to hold the response data
        std::string readBuffer;

        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the CURL request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Parse the response JSON and extract positions
        if (!readBuffer.empty())
        {
            auto responseJson = json::parse(readBuffer);
            std::cout << responseJson << std::endl;
            if (responseJson.contains("result"))
            {
                // Assuming positions are in the "result" field
                for (const auto &item : responseJson["result"])
                {
                    std::cout << "Instrument: " << item["instrument_name"] << "\n"
                              << "Direction: " << item["direction"] << "\n"
                              << "Size: " << item["size"] << "\n"
                              << "Mark Price: " << item["mark_price"] << "\n"
                              << "Floating P/L: " << item["floating_profit_loss"] << "\n"
                              << "----------------------" << std::endl;
                }
            }
            else
            {
                std::cerr << "Error fetching positions: " << responseJson["error"]["message"] << std::endl;
            }
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

// Function to get a list of open orders by currency
void getOpenOrdersByCurrency(const std::string &currency, const std::string &accessToken)
{
    std::string url = "https://test.deribit.com/api/v2/private/get_open_orders_by_currency";

    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "private/get_open_orders_by_currency"},
        {"params", {{"currency", currency}}}};

    std::string response = sendRequest(url, payload, accessToken);
    auto responseJson = json::parse(response);
    std::cout << responseJson.dump(4) << std::endl; // Print the JSON response for debugging

    if (responseJson.contains("result"))
    {
        const auto &orders = responseJson["result"];
        if (orders.empty())
        {
            std::cout << "No open orders for currency: " << currency << std::endl;
            return;
        }
        std::cout << "Open Orders for Currency: " << currency << std::endl;
        for (const auto &item : orders)
        {
            std::cout << "Instrument: " << item["instrument_name"] << "\n"
                      << "Direction: " << item["direction"] << "\n"
                      << "Size: " << item["amount"] << "\n"
                      << "Price: " << item["price"] << "\n"
                      << "----------------------" << std::endl;
        }
    }
    else if (responseJson.contains("error"))
    {
        std::cerr << "Error fetching open orders: " << responseJson["error"]["message"] << std::endl;
    }
    else
    {
        std::cerr << "Unexpected response: " << response << std::endl;
    }
}


void getUserTradesByCurrency(const std::string &currency, const std::string &accessToken)
{
    std::string url = "https://test.deribit.com/api/v2/private/get_user_trades_by_currency";

    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "private/get_user_trades_by_currency"},
        {"params", {{"currency", currency}, {"kind", "future"}, {"count", 10}}}, // Customize "kind" and "count" as needed
        {"id", 1}};

    std::string response = sendRequest(url, payload, accessToken);
    auto responseJson = json::parse(response);
    std::cout<<responseJson<<std::endl;
    if (responseJson.contains("result"))
    {
        if(responseJson["result"]["trades"].empty()){
            std::cout<<"No trades for currency: "<<currency<<std::endl;
            return;
        }
        std::cout << "Trades for Currency: " << currency << std::endl;
        for (const auto &item : responseJson["result"]["trades"])
        {
            std::cout << "Instrument: " << item["instrument_name"] << "\n"
                      << "Direction: " << item["direction"] << "\n"
                      << "Size: " << item["amount"] << "\n"  // Use "amount" instead of "size" for trades
                      << "Price: " << item["price"] << "\n"
                      << "----------------------" << std::endl;
        }
    }
    else
    {
        std::cerr << "Error fetching trades: " << responseJson["error"]["message"] << std::endl;
    }
}
