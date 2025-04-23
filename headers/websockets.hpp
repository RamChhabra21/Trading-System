// header/websockets.hpp

#ifndef WEBSOCKETS_HPP
#define WEBSOCKETS_HPP

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <iostream>
#include <string>
#include <unordered_set>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<boost::asio::ssl::context> ssl_context_ptr;

class DeribitWebSocketClient {
public:
    client ws_client;
    bool isbroad=false;
    // Using std::shared_ptr for better memory management
    std::shared_ptr<websocketpp::connection<websocketpp::config::asio_tls_client>> connection_handle;
    bool is_authenticated = false;
    std::unordered_set<std::string> active_subscriptions;

    // Store client credentials
    std::string client_id;
    std::string client_secret;

    // Constructor to initialize client_id and client_secret
    DeribitWebSocketClient(const std::string& client_id, const std::string& client_secret);

    // Method to connect to WebSocket server
    void connect(const std::string& uri);

    // Methods for subscription management
    void subscribeToSymbol(const std::string& symbol);
    void unsubscribeFromSymbol(const std::string& symbol);
    void printActiveSubscriptions();
    void receiveRealTimeBroadcast();
    

    // WebSocket event handlers
    void on_open(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg);
    void on_close(websocketpp::connection_hdl hdl);
    void on_fail(websocketpp::connection_hdl hdl);
    
    // Methods for WebSocket lifecycle management
    void startWebSocket();
    void stopWebSocket();
    
    // Authentication and TLS initialization
    void authenticate();
    ssl_context_ptr on_tls_init(websocketpp::connection_hdl hdl);
};

#endif // WEBSOCKETS_HPP
