#include "net_client.h"

bool net_bridge::c_client::connect( bool startthread )
{
    if ( !client_socket ) {
#if DEBUG_LOGS
        printf( "c_client::connect | client_socket is not initialized. \n" );
#endif
        return false;
    }

    is_running = true;
    return client_socket->connect_to_server( );
}

void net_bridge::c_client::start_listeners( )
{
    client_thread = std::thread( &net_bridge::c_client::client_handler, this );
    packet_thread = std::thread( &net_bridge::c_client::packet_handler, this );
    send_thread = std::thread( &net_bridge::c_client::send_handler, this );
}

void net_bridge::c_client::packet_handler( )
{
    while ( !client_socket || !client_socket->get_socket( ) )
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
    auto base_sock = client_socket->get_socket( );

    container.new_client( client_socket->get_socket( ) );
    std::this_thread::sleep_for( std::chrono::milliseconds( 2500 ) );

    while ( is_running ) {

        auto packets = container.get_packets( );


        if ( !packets[ base_sock ] )
            packets[ base_sock ] = new std::deque<net_bridge::c_packet>( );


        auto& deq = packets[ base_sock ];

        while ( !deq->empty( ) ) {

            net_bridge::c_packet& packet = deq->front( );

            if ( packet.info.state != e_packet_state::completed ) {
                break;
            }


            if ( !packet.info.is_handled ) {
                packet.info.is_handled = true;



                printf( "Handling Packet [%d] \n", deq->size( ) );
                on_handle_packet( *client_socket, packet );

                deq->pop_front( );
            }
            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        }
    }
}

void net_bridge::c_client::send_handler( )
{
    std::this_thread::sleep_for( std::chrono::milliseconds( 2500 ) );

    while ( is_running ) {
        while ( !packets_to_send.empty( ) ) {
            auto& obj = packets_to_send.front( );

            client_socket->send_bytes( obj.get_buffer( ), obj.get_buffer_length( ) );

            packets_to_send.pop_front( );
        }
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
    }
}

void net_bridge::c_client::client_handler( )
{
    while ( !client_socket || !client_socket->get_socket( ) )
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
    size_t buf_size = 1024 * 1024;
    read_buffer = new BYTE[ buf_size ];

    auto csock = client_socket->get_socket( );

    std::this_thread::sleep_for( std::chrono::milliseconds( 2500 ) );
    while ( is_running ) {

        ZeroMemory( read_buffer, buf_size );

        int bytes_received = recv( csock, ( char* )read_buffer, buf_size, 0 );
        printf( "Received Bytes [%d]", bytes_received );
        if ( bytes_received <= 0 ) {

            on_disconnect( );

            is_running = false;

        }
        else {
            container.on_data_received( csock, net_bridge::c_array( ( int8_t* )read_buffer, bytes_received ) );
        }


    }
}