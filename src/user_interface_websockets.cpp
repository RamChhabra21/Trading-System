#include "user_interface_websockets.hpp"
#include "credentials.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <future>
#include "websockets.hpp" 

std::mutex client_mutex;

// WebSocket interface to handle user input
void websocketInterface()
{
    std::string uri = "wss://test.deribit.com/ws/api/v2";
    std::string symbol;

    // Create WebSocket client instance with credentials
    DeribitWebSocketClient client(client_id, client_secret);

    // Launch the WebSocket client in a separate thread asynchronously
    auto ws_task = std::async(std::launch::async, [&client, &uri]()
                              {
        try {
            client.connect(uri);  // Connect to WebSocket server
            client.startWebSocket();  // Start the WebSocket event loop
        } catch (const std::exception &e) {
            std::cerr << "Error during WebSocket operation: " << e.what() << std::endl;
        } });

    // Process user input in the main thread (UI menu)
    while (true)
    {
        std::cout << "Select an option:\n";
        std::cout << "1. Subscribe to a Symbol\n";
        std::cout << "2. Unsubscribe from a Symbol\n";
        std::cout << "3. View Real-Time Broadcast Data\n";
        std::cout << "4. View Current Subscriptions\n";
        std::cout << "5. Exit WebSocket Interface\n";
        std::cout << "Enter your choice: ";
        int selection;
        if (!(std::cin >> selection))
        {
            std::cerr << "Invalid input, exiting...\n";
            break;
        }

        switch (selection)
        {
        case 1:
            std::cout << "Enter symbol to subscribe to: ";
            std::cin >> symbol;
            try
            {
                std::unique_lock<std::mutex> lock(client_mutex);
                client.subscribeToSymbol(symbol);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error subscribing to symbol: " << e.what() << std::endl;
            }
            client.printActiveSubscriptions();
            break;

        case 2:
            std::cout << "Enter symbol to unsubscribe from: ";
            std::cin >> symbol;
            try
            {
                std::unique_lock<std::mutex> lock(client_mutex);
                client.unsubscribeFromSymbol(symbol);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error unsubscribing from symbol: " << e.what() << std::endl;
            }
            client.printActiveSubscriptions();
            break;
        case 3:
        {
            std::cout<<"Receiving real-time data for the following symbols: "<<std::endl;
            for(const auto &symbol: client.active_subscriptions) 
            {
                std::cout<<symbol<<std::endl;
            }
            try
            {
                std::lock_guard<std::mutex> lock(client_mutex);
                client.isbroad=true;
                client.receiveRealTimeBroadcast();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error receiving real-time data: " << e.what() << std::endl;
            }
            break;
        }
        case 4:
        {
            std::cout<<" Active Subscriptions are as follows: "<<std::endl;
            for(const auto &symbol: client.active_subscriptions) {
                std::cout<<symbol<<std::endl;
            }
            break;
        }
        case 5:{
            std::cout << "Exiting WebSocket interface...\n";
            try
            {
                // std::unique_lock<std::mutex> lock(client_mutex);
                client.stopWebSocket();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error during WebSocket shutdown: " << e.what() << std::endl;
            }
            // Wait for the WebSocket thread to finish
            if (ws_task.valid())
            {
                ws_task.get();
            }
            return;
        }
        default:
            std::cout << "Invalid selection, please try again.\n";
            break;
        }
    }
}
