#include <iostream>
#include <chrono>
#include <fstream>
#include <thread>
#include <future>
#include <string>
#include <mutex>
#include "headers/order_management.hpp"
#include "headers/websockets.hpp"
#include "headers/market_data.hpp"
#include "headers/credentials.hpp"

void placeOrderCheck()
{
    // placing an order with sample values
    placeOrder("10", accessToken, "10", "ALGO_USDC-PERPETUAL", "market");
}

void receiveMarketData()
{
    // receiving market data for a sample instrument
    getOrderBook(accessToken, "ALGO_USDC-PERPETUAL");
}

void subscribetosymbol(const std::string &symbol)
{
    std::string uri = "wss://test.deribit.com/ws/api/v2";
    DeribitWebSocketClient client(client_id, client_secret);
    client.connect(uri);     // Connect to WebSocket server
    client.startWebSocket(); // Start the WebSocket event loop
    client.stopWebSocket();
}

// Function to log benchmark results to a file
void logBenchmarkResult(const std::string &metric, long long latency)
{
    std::ofstream log_file("benchmark_log.txt", std::ios_base::app);
    if (log_file.is_open())
    {
        log_file << metric << ": " << latency << " ms" << std::endl;
        log_file.close();
    }
}

// Latency Benchmarking Functions (asynchronous)
std::future<long long> benchmarkOrderPlacement()
{
    return std::async(std::launch::async, []() -> long long
                      {  
        auto start = std::chrono::high_resolution_clock::now();
        placeOrderCheck();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); });
}

std::future<long long> benchmarkMarketDataProcessing()
{
    return std::async(std::launch::async, []() -> long long
                      {  
        auto start = std::chrono::high_resolution_clock::now();
        receiveMarketData();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); });
}

std::future<long long> benchmarkWebSocketMessagePropagation()
{
    return std::async(std::launch::async, []() -> long long
                      {  
        auto start = std::chrono::high_resolution_clock::now();
        subscribetosymbol("trades.BTC-USD");
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); });
}

std::future<long long> benchmarkEndToEndTradingLoop()
{
    return std::async(std::launch::async, []() -> long long
                      { 
        auto start = std::chrono::high_resolution_clock::now();
        placeOrderCheck();
        receiveMarketData();
        subscribetosymbol("trades.BTC-USD");
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); });
}

int main()
{
    std::string clientId = client_id;
    std::string clientSecret = client_secret;

    accessToken = getAccessToken(clientId, clientSecret);

    if (accessToken.empty())
    {
        std::cerr << "Unable to obtain access token." << std::endl;
        return 0;
    }
    std::cout << "Starting latency benchmarking...\n";

    // Run all benchmarks asynchronously
    auto orderLatencyFuture = benchmarkOrderPlacement();
    auto marketDataLatencyFuture = benchmarkMarketDataProcessing();
    auto wsLatencyFuture = benchmarkWebSocketMessagePropagation();
    auto tradingLoopLatencyFuture = benchmarkEndToEndTradingLoop();

    // Wait for results from all tasks asynchronously
    auto order_latency = orderLatencyFuture.get();
    auto market_data_latency = marketDataLatencyFuture.get();
    auto ws_message_latency = wsLatencyFuture.get();
    auto total_latency = tradingLoopLatencyFuture.get();

    // Output the results
    std::cout << "Order Placement Latency: " << order_latency << " ms" << std::endl;
    std::cout << "Market Data Processing Latency: " << market_data_latency << " ms" << std::endl;
    std::cout << "WebSocket Message Propagation Delay: " << ws_message_latency << " ms" << std::endl;
    std::cout << "End-to-End Trading Loop Latency: " << total_latency << " ms" << std::endl;

    std::ofstream log_file("benchmark_log.txt", std::ios_base::trunc);
    // Log the results
    logBenchmarkResult("Order Placement Latency", order_latency);
    logBenchmarkResult("Market Data Processing Latency", market_data_latency);
    logBenchmarkResult("WebSocket Message Propagation Delay", ws_message_latency);
    logBenchmarkResult("End-to-End Trading Loop Latency", total_latency);

    std::cout << "Benchmarking completed. Results saved to benchmark_log.txt.\n";
    return 0;
}