# NetBridge
NetBridge is an open source TCP communication library, designed to provide a reliable and efficient means of transmitting data between applications over a network. It provides a simple API for establishing connections, sending and receiving data, and handling errors and timeouts.

NetBridge is built C++ and is synchronous. Has only been tested on Windows.

NetBridge is distributed under the MIT license, which means that it can be freely used, modified, and distributed, both for commercial and non-commercial purposes. Contributions and feedback are welcome and encouraged.

![](https://i.imgur.com/iCphBRJ.png)
![](https://i.imgur.com/cTYYaWG.png)

# Setting up project
You want to add the path to the NetBridge folder to your include directories. (Configuration Properties -> VC++ Directories)
And add path to the Release folder where the NetBridge.lib is located to your library directories. (Configuration Properties -> VC++ Directories)
Be sure to add NetBridge.lib to the Additional Dependencies. (Configuration Properties -> Linker -> Input)

# Server Example
This is an example of the server side with the library, We make a user class and override the virtuals.
```c++
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
```
Usage is done like so
```cpp
int main( )
{
    // Set the title of the console window
    SetConsoleTitleA( "NetBridge Synchronous TCP Server Library" );

    // Initialize the Winsock library
    WSAData data;
    WORD ver = MAKEWORD( 2, 2 );
    int wsResult = WSAStartup( ver, &data );

    // Create a new instance of the server
    Log( "Starting Server [{}]", SERVER_PORT );
    std::unique_ptr<net::net_server> main_server = std::make_unique<net::net_server>( SERVER_PORT );

    // Check if the server started successfully
    if ( main_server->begin_listening( ) )
        Log( "Started server." );
    else 
        Log( "Failed to start server." );

    // Run an infinite loop to keep the program running
    while ( 1 ) {
        std::cin.get( );
        // Sleep for 1 millisecond to prevent the program from using too much CPU
        Sleep( 1 );
    
```
When using this libary only file needed to include is net_server.h this includes all other files needed for this project to build and operate without error.(For both client & server)
When calling begin_listening you can pass in a bool that'll start threads for client handling, packet handling, and send handling.
# Client Example
Client is the same way need to make a user class like so
```cpp
class client : public net_bridge::c_client
{
public:
    client( std::string ip, uint16_t p ) : net_bridge::c_client( ip, p ) { }

    virtual void on_handle_packet( net_bridge::c_socket& client_socket, net_bridge::c_packet& message ) override;
    virtual void on_disconnect( ) override;
};

void client::on_handle_packet( net_bridge::c_socket& client_socket, net_bridge::c_packet& message )
{
}

void client::on_disconnect( )
{
}
```
Usage is done like so
```cpp
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
```





