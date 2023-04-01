#include <net_bridge/server/net_server.h>
#pragma comment(lib, "NetBridge")

#define SERVER_PORT 2048

namespace net {

    class client : public net_bridge::c_client
    {
    public:
        client( std::string ip, uint16_t p ) : net_bridge::c_client( ip, p ) { }
     
        virtual void on_handle_packet( net_bridge::c_socket& client_socket, net_bridge::c_packet& message ) override;
        virtual void on_disconnect() override;
    };

    void client::on_handle_packet( net_bridge::c_socket& client_socket, net_bridge::c_packet& message )
    {
    }

    void client::on_disconnect( )
    {
    }
};

int main( )
{
    // Set the title of the console window
    SetConsoleTitleA( "NetBridge Synchronous TCP CLIENT TEST" );

    // Initialize the Winsock library
    WSAData data;
    WORD ver = MAKEWORD( 2, 2 );
    int wsResult = WSAStartup( ver, &data );
    // Create a new instance of the client and start listening for incoming data
    std::unique_ptr<net::client> game_client = std::make_unique<net::client>( "127.0.0.1", SERVER_PORT );
   
    game_client->start_listeners( );

    // Attempt to connect to the server
    if ( game_client->connect( ) )
        Log( "Connected to server" );
   
    // Wait for users input  
    system( "pause" );
    
    return -1;
}
