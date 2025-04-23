#include "user_interface.hpp"
#include "user_interface_websockets.hpp"  // Include WebSocket UI header
#include <iostream>

int main() {
    int selection;

    while (true) {
        std::cout << "Select the interface you want to use:\n";
        std::cout << "1. Order User Interface\n";
        std::cout << "2. WebSocket User Interface\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> selection;

        switch (selection) {
            case 1:
                printMainMenu();  // order UI
                int order_selection;
                std::cin >> order_selection;
                handleUserSelection(order_selection);  // Handle order UI selection
                break;

            case 2:
                std::cout << "Starting WebSocket Interface...\n";
                websocketInterface();  // Function to handle WebSocket UI
                break;

            case 3:
                std::cout << "Exiting program...\n";
                return 0;

            default:
                std::cout << "Invalid selection, please try again.\n";
                break;
        }
    }

    return 0;
}
