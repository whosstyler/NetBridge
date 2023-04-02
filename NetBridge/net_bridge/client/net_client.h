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
            client_socket = std::make_shared<c_socket>( ip, p );
        }

        bool connect( bool start_thread = true );
    public:
        bool running( ) { return is_running; }
        void stop( ) { is_running = false; }

        std::shared_ptr<c_socket> get_socket( ) { return client_socket; }

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
        std::shared_ptr<c_socket> client_socket = nullptr;
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
            Log( "%s | New client connected! [%llx] adding to container.", __FUNCTION__, sock_id );         
            clients[ sock_id ] = sock;
        }

        void on_disconnected( c_socket sock ) {
            auto sock_id = sock.get_socket( );          
            Log( "%s | Client disconnected [%llx] removing from container.", __FUNCTION__, sock_id );         
            if ( clients.find( sock_id ) != clients.end( ) )
                clients.erase( sock_id );
        }
        void on_disconnected( std::uint64_t sock_id ) {
            Log( "%s | Client disconnected [%llx] removing from container.", __FUNCTION__, sock_id );

            if ( clients.find( sock_id ) != clients.end( ) ) {
                auto m_sock = clients[ sock_id ];
                m_sock.close_socket( );
                clients.erase( sock_id );
                }
            }

        c_socket get_client( std::uint64_t m_socket ) {
            if ( clients.find( m_socket ) == clients.end( ) ) {
                Log( "%s | Failed to remove client from container [%llx]. (Client doesnt exist.)", __FUNCTION__, m_socket );
                return c_socket( 0 );
            }
            return clients[ m_socket ];
        }

    private:
        std::map<std::uint64_t, c_socket> clients;
        };
}