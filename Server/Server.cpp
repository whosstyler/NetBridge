#include <net_bridge/server/net_server.h>
#pragma comment(lib, "NetBridge")
#define SERVER_PORT 2048

namespace net {
    class net_server : public net_bridge::c_tcp_server
    {
    public:
        net_server( std::uint16_t p ) : net_bridge::c_tcp_server( p ) { }

        virtual void on_handle_packet( net_bridge::c_socket& client_socket, net_bridge::c_packet& message ) override;
        virtual bool on_client_connected( net_bridge::c_socket& client_socket ) override;
        virtual void on_client_connection_success( net_bridge::c_socket& client_socket ) override;
    private:
        virtual void on_client_disconnected( std::uint64_t client_socket ) override;
    };
    void net_server::on_handle_packet( net_bridge::c_socket& client_socket, net_bridge::c_packet& message )
    {

    }
    bool net_server::on_client_connected( net_bridge::c_socket& client_socket )
    {
        return true;
    }
    void net_server::on_client_connection_success( net_bridge::c_socket& client_socket )
    {
    }
    void net_server::on_client_disconnected( std::uint64_t client_socket )
    {
    }
}

int main( )
{
    // Set the title of the console window
    SetConsoleTitleA( "NetBridge Synchronous TCP Server Library" );

    // Initialize the Winsock library
    WSAData data;
    WORD ver = MAKEWORD( 2, 2 );
    int wsResult = WSAStartup( ver, &data );

    // Create a new instance of the server
    spdlog::info( "Starting Server [{}]", SERVER_PORT );
    auto main_server = new net::net_server( SERVER_PORT );

    // Check if the server started successfully
    if ( main_server->begin_listening( ) ) {
        spdlog::info( "Started server." );
    }
    else spdlog::error( "Failed to start server." );

    // Run an infinite loop to keep the program running
    while ( 1 ) {
        std::cin.get( );
        // Sleep for 1 millisecond to prevent the program from using too much CPU
        Sleep( 1 );
    }
}
