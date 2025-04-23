#include "user_interface.hpp"
#include "credentials.hpp"
#include "order_management.hpp"
#include "market_data.hpp"
#include <iostream>
#include <string>


void printMainMenu() {
    std::cout << "Welcome to the Deribit Trading System!" << std::endl;
    std::cout << "Choose from the following Options" << std::endl;
    std::cout << "1. Place Order" << std::endl;
    std::cout << "2. Cancel Order" << std::endl;
    std::cout << "3. Modify Order" << std::endl;
    std::cout << "4. Get OrderBook" << std::endl;
    std::cout << "5. View Current Positions" << std::endl;  
    std::cout << "6. Get Open Orders by Currency" << std::endl; 
    std::cout << "7. Get Orders by Currency" << std::endl;
    std::cout << "8. Get Instruments" << std::endl;
    std::cout << "9. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

void handleUserSelection(int selection) {
    std::string clientId = client_id;
    std::string clientSecret = client_secret;

    std::string accessToken = getAccessToken(clientId, clientSecret);

    if (accessToken.empty()) {
        std::cerr << "Unable to obtain access token." << std::endl;
        return;
    }

    switch (selection) {
        case 1: {
            std::string price,amount, instrument, orderType;
            std::cout << "Enter price: ";
            std::cin >> price;
            std::cout << "Enter amount: ";
            std::cin >> amount;
            std::cout << "Enter instrument: ";
            std::cin >> instrument;
            std::cout << "Enter order type (market/limit): "; // any valid order type is acceptable  
            std::cin >> orderType;

            placeOrder(price, accessToken, amount, instrument, orderType);
            break;
        }
        case 2: {
            std::string orderId;
            std::cout << "Enter order ID to cancel: ";
            std::cin >> orderId;
            cancelOrder(accessToken, orderId);
            break;
        }
        case 3: {
            std::string orderId;
            int amount;
            double price;
            std::cout << "Enter order ID to modify: ";
            std::cin >> orderId;
            std::cout << "Enter new amount: ";
            std::cin >> amount;
            std::cout << "Enter new price: ";
            std::cin >> price;
            modifyOrder(accessToken, orderId, amount, price);
            break;
        }
        case 4: {
            std::string instrument;
            std::cout << "Enter instrument to get order book: ";
            std::cin >> instrument;
            getOrderBook(accessToken, instrument);
            break;
        }
        case 5: {
            getCurrentPositions(accessToken);
            break;
        }
        case 6: {
            std::cout << "Enter currency to get orders: ";
            std::string currency;
            std::cin>>currency;
            getOpenOrdersByCurrency(currency,accessToken);
            break;
        }
        case 7: {
            std::cout << "Enter currency to get orders: ";
            std::string currency;
            std::cin>>currency;
            getUserTradesByCurrency(currency, accessToken);
            break;
        }
        case 8: {
            std::vector<std::string> instruments;
            instruments = getInstruments(accessToken);
            std::cout << "Available Instruments:\n";
            for (const auto& instrument : instruments) {
                std::cout << instrument << std::endl;
            }
            break;
        }
        case 9: {
            std::cout << "Exiting..." << std::endl;
            return;
        }
        default:
            std::cout << "Invalid selection, please try again." << std::endl;
            break;
    }
}
