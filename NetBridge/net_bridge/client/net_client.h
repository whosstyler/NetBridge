#pragma once
#include "../../net_bridge.h"
#include "../socket/net_socket.h"
#include "../packet/net_packet.h"

namespace net_bridge
{
	class c_client
	{
	public:
        inline c_client( std::string ip, uint16_t p )
        {
            client_socket = new c_socket( ip, p );
        }

        bool connect( bool start_thread = true );
    public:
        bool running( ) { return is_running; }
        void stop( ) { is_running = false; }

        c_socket* get_socket( ) { return client_socket; }

        void start_listeners( );

    public:
        virtual void on_handle_packet( c_socket& client_socket, c_packet& message ) = 0;

        virtual void on_disconnect( ) = 0;

    private:
        void packet_handler( );
        void send_handler( );
        void client_handler( );

    private:
        c_packet_container container;
        c_socket* client_socket = nullptr;
        std::deque<c_array> packets_to_send;

        bool is_running = true;

        std::thread send_thread;
        std::thread packet_thread;
        std::thread client_thread;

        BYTE* read_buffer = 0;
    };

    class c_client_container
    {
    public:
        inline c_client_container( ) {}


        void on_connected( c_socket sock ) {
            auto sock_id = sock.get_socket( );
#if DEBUG_LOGS
            printf( "c_client_container::on_connected | New client connected! [%d] adding to container.\n", sock_id );
#endif

            clients[ sock_id ] = sock;
        }

        void on_disconnected( c_socket sock ) {
            auto sock_id = sock.get_socket( );
#if DEBUG_LOGS
            printf( "c_client_container::on_connected | Client disconnected [%d] removing from container.\n", sock_id );
#endif
            if ( clients.find( sock_id ) != clients.end( ) )
                clients.erase( sock_id );
        }
        void on_disconnected( std::uint64_t sock_id ) {
#if DEBUG_LOGS
            printf( "c_client_container::on_connected | Client disconnected [%d] removing from container.\n", sock_id );
#endif
            if ( clients.find( sock_id ) != clients.end( ) ) {
                auto m_sock = clients[ sock_id ];

                m_sock.close_socket( );
                clients.erase( sock_id );
            }
            }

        c_socket get_client( std::uint64_t m_socket ) {
            if ( clients.find( m_socket ) == clients.end( ) ) {
#if DEBUG_LOGS
                printf( "c_client_container::get_client | Failed to remove client from container [%d]. (Client doesnt exist.) \n", m_socket );

#endif

                return c_socket( 0 );
            }

            return clients[ m_socket ];
        }

    private:
        std::map<std::uint64_t, c_socket> clients;
        };
}