#pragma once
#include "../../net_bridge.h"
#include "../socket/net_socket.h"
#include "../packet/net_packet.h"
#include "../client/net_client.h"

namespace net_bridge
{
	class c_tcp_server
	{
	public:
        inline c_tcp_server( ) {

        }
     
        inline c_tcp_server( uint16_t p )
        {
            server_socket = std::make_unique<c_socket>(p);
        }

		bool begin_listening( bool start_thread = true );
    public:

        virtual void on_handle_packet( c_socket& client_socket, net_bridge::c_packet& message ) = 0;

        virtual bool on_client_connected( c_socket& client_socket ) = 0;

        virtual void on_client_connection_success( c_socket& client_socket ) = 0;

        virtual void on_client_disconnected( std::uint64_t client_socket ) = 0;

    public:
        bool running( ) { return is_running; }

        void stop( ) { is_running = false; }

        void send_to_client( c_socket client, net_bridge::c_array arr );


    private:
        void packet_handler( );
        void send_handler( );
        void client_handler( );
    private:
        std::unique_ptr<c_socket> server_socket = nullptr;

        net_bridge::c_client_container container;
        net_bridge::c_packet_container packet_container;
        bool is_running = false;

        std::thread send_thread;
        std::thread packet_thread;

        std::thread client_thread;

        std::deque<std::pair<c_socket, net_bridge::c_array>> packets_to_send;

        BYTE* read_buffer = 0;
	};
}