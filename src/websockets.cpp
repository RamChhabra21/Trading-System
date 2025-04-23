#include "websockets.hpp"
#include "credentials.hpp"
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <chrono>

std::mutex connection_mutex;
std::thread ws_thread;

DeribitWebSocketClient::DeribitWebSocketClient(const std::string &client_id, const std::string &client_secret)
    : client_id(client_id), client_secret(client_secret) {
    ws_client.init_asio();
    ws_client.start_perpetual();  // Keep the event loop running asynchronously
    ws_client.set_tls_init_handler([this](websocketpp::connection_hdl hdl) { return on_tls_init(hdl); });
    ws_client.set_open_handler([this](websocketpp::connection_hdl hdl) { on_open(hdl); });
    ws_client.set_message_handler([this](websocketpp::connection_hdl hdl, client::message_ptr msg) { on_message(hdl, msg); });
    ws_client.set_close_handler([this](websocketpp::connection_hdl hdl) { on_close(hdl); });
    ws_client.set_fail_handler([this](websocketpp::connection_hdl hdl) { on_fail(hdl); });
}

ssl_context_ptr DeribitWebSocketClient::on_tls_init(websocketpp::connection_hdl) {
    ssl_context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
                         boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 |
                         boost::asio::ssl::context::single_dh_use);
    } catch (std::exception &e) {
        std::cerr << "Error in TLS init: " << e.what() << std::endl;
    }
    return ctx;
}

void DeribitWebSocketClient::startWebSocket() {
    try {
        ws_thread = std::thread([this]() {
            ws_client.run();
        });
        std::cout << "WebSocket client started..." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "WebSocket run error: " << e.what() << std::endl;
    }
}

void DeribitWebSocketClient::stopWebSocket() {
    try {
        ws_client.stop_perpetual();
        std::cout << "Stopping WebSocket client..." << std::endl;
        ws_client.stop();
        std::cout << "Joining thread" << std::endl;
        if (ws_thread.joinable()) {
            ws_thread.join();  // Ensure the thread finishes before exiting
        }
        std::cout << "WebSocket client stopped gracefully." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error while stopping WebSocket: " << e.what() << std::endl;
    }
}

void DeribitWebSocketClient::on_open(websocketpp::connection_hdl hdl) {
    std::cout << "Connected to WebSocket!" << std::endl;
    connection_handle = ws_client.get_con_from_hdl(hdl);  // Store the connection as shared_ptr
    authenticate();
}

void DeribitWebSocketClient::on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
    try {
        auto payload = msg->get_payload();
        if(isbroad) std::cout << "Message received: " << payload << std::endl;
        // Add any additional logic to handle incoming messages
    } catch (const std::exception &e) {
        std::cerr << "Error processing message: " << e.what() << std::endl;
    }
}

void DeribitWebSocketClient::on_close(websocketpp::connection_hdl hdl) {
    std::cout << "Connection closed!" << std::endl;
    connection_handle.reset();  // Reset the handle to avoid reuse
}

void DeribitWebSocketClient::on_fail(websocketpp::connection_hdl hdl) {
    std::cerr << "Connection failed!" << std::endl;
    connection_handle.reset();  // Reset the handle to avoid reuse
}

void DeribitWebSocketClient::connect(const std::string &uri) {
    websocketpp::lib::error_code ec;
    client::connection_ptr con = ws_client.get_connection(uri, ec);
    if (ec) {
        std::cerr << "Connection error: " << ec.message() << std::endl;
        return;
    }
    ws_client.connect(con);
}

void DeribitWebSocketClient::authenticate() {
    if (connection_handle) {
        std::string auth_message = R"({
            "jsonrpc": "2.0",
            "id": 1,
            "method": "public/auth",
            "params": {
                "grant_type": "client_credentials",
                "client_id": ")" + client_id +
                                    R"(",
                "client_secret": ")" + client_secret +
                                    R"("
            }
        })";

        ws_client.send(connection_handle, auth_message, websocketpp::frame::opcode::text);
    } else {
        std::cerr << "Error: No valid connection handle for authentication." << std::endl;
    }
}

void DeribitWebSocketClient::subscribeToSymbol(const std::string &symbol) {
    std::cout << "Subscribing to: " << symbol << std::endl;
    if (connection_handle) {
        auto conn = connection_handle;
        auto websocket_conn = std::static_pointer_cast<websocketpp::connection<websocketpp::config::asio_tls_client>>(conn);

        if (websocket_conn) {
            std::string subscribe_message = R"({
                "jsonrpc": "2.0",
                "id": 1,
                "method": "public/subscribe",
                "params": {
                    "channels": [")" + symbol + R"("]
                }
            })";
            ws_client.send(connection_handle, subscribe_message, websocketpp::frame::opcode::text);
            active_subscriptions.insert(symbol);
            std::cout << "Subscribed to: " << symbol << std::endl;
        } else {
            std::cerr << "Error: Failed to lock WebSocket connection" << std::endl;
        }
    } else {
        std::cerr << "Error: No valid connection handle for subscribing." << std::endl;
    }
}

void DeribitWebSocketClient::unsubscribeFromSymbol(const std::string &symbol) {
    if (connection_handle) {
        std::string unsubscribe_message = R"({
            "jsonrpc": "2.0",
            "id": 3,
            "method": "public/unsubscribe",
            "params": {
                "channels": [")" + symbol + R"("]
            }
        })";
        ws_client.send(connection_handle, unsubscribe_message, websocketpp::frame::opcode::text);
        active_subscriptions.erase(symbol);
        std::cout << "Unsubscribed from: " << symbol << std::endl;
    } else {
        std::cerr << "Error: No valid connection handle for unsubscribing." << std::endl;
    }
}

void DeribitWebSocketClient::printActiveSubscriptions() {
    std::cout << "Currently subscribed to the following channels:" << std::endl;
    if (active_subscriptions.empty()) {
        std::cout << "No active subscriptions." << std::endl;
    } else {
        for (const auto &symbol : active_subscriptions) {
            std::cout << " - " << symbol << std::endl;
        }
    }
}

void DeribitWebSocketClient::receiveRealTimeBroadcast() {
    std::cout << "Receiving real-time data" << std::endl;

    while (true) {
        if (!connection_handle) {
            std::cerr << "WebSocket connection is closed. Exiting.\n";
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Sleep to avoid high CPU usage
        // Real-time data processing will happen in on_message handler
    }

    std::cout << "Exited real-time data listener.\n";
}
