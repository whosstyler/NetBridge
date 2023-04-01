#include "net_socket.h"

bool net_bridge::c_socket::connect_to_server( )
{
    sock = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock == INVALID_SOCKET ) {
        Log( "%s | failed to initialize socket", __FUNCTION__ );
        return false;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons( port );
    inet_pton( AF_INET, target_server_ip.c_str( ), &hint.sin_addr );
    
    int res = connect( sock, ( sockaddr* )&hint, sizeof( hint ) );
    if ( res == SOCKET_ERROR ) {
        Log( "%s | can't connect to server. (server offline?)", __FUNCTION__ );
        return false;
    }

    return true;
}

bool net_bridge::c_socket::server_begin_listening( )
{
    sock = socket( AF_INET, SOCK_STREAM, 0 );

    if ( sock == INVALID_SOCKET ) 
        Log( "%s | failed to initialize socket", __FUNCTION__ );

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons( port );
    hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 

    bind( sock, ( sockaddr* )&hint, sizeof( hint ) );

    // Tell Winsock the socket is for listening 
    listen( sock, SOMAXCONN );

    return true;
}

void net_bridge::c_socket::send_bytes( int8_t* bytes, size_t size )
{
    Log( "sending bytes" );
    send( sock, ( char* )bytes, size, 0 );
}
