#include "net_server.h"

bool net_bridge::c_tcp_server::begin_listening( bool start_thread )
{	
	if ( !server_socket ) {
		Log( "%s | server socket is not initialized!", __FUNCTION__ );
		return false;
	}

	if ( !server_socket->server_begin_listening( ) ) {
		Log( "%s | c_socket::server_begin_listening failed.", __FUNCTION__ );
		return false;
	}

	Log( "%s | success.", __FUNCTION__ );

	is_running = true;
	if ( start_thread ) {
		client_thread = std::thread( &net_bridge::c_tcp_server::client_handler, this );
		packet_thread = std::thread( &net_bridge::c_tcp_server::packet_handler, this );
		send_thread = std::thread( &net_bridge::c_tcp_server::send_handler, this );
	}

	return true;
}

void net_bridge::c_tcp_server::packet_handler( )
{
	while ( is_running ) {
		auto packets = packet_container.get_packets( );
		for ( auto& fclient : packets ) {

			auto& deq = fclient.second;
			while ( !deq->empty( ) ) {
				net_bridge::c_packet& packet = deq->front( );

				if ( packet.info.state != e_packet_state::completed ) 
					break;

				if ( !packet.info.is_handled ) {
					packet.info.is_handled = true;

					auto client_sock = container.get_client( fclient.first );
					if ( client_sock.get_socket( ) == 0 ) {
						Log( "%s | failed to get client_socket [%d]", __FUNCTION__, fclient.first );
						deq->clear( );
						continue;
					}

					Log( "Handling Packet [%d]", deq->size( ) );
					on_handle_packet( client_sock, packet );
					deq->pop_front( );
				}
			}
		}
		std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
	}
}

void net_bridge::c_tcp_server::send_handler( )
{
	while ( is_running ) {
		while ( !packets_to_send.empty( ) ) {
			auto& obj = packets_to_send.front( );
			obj.first.send_bytes( obj.second.get_buffer( ), obj.second.get_buffer_length( ) );
			packets_to_send.pop_front( );
		}
		std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
	}
}

void net_bridge::c_tcp_server::client_handler( )
{
	size_t buf_size = 1024 * 1024;
	read_buffer = new BYTE[ buf_size ];
	fd_set master;
	auto base_server_socket = server_socket->get_socket( );
	FD_ZERO( &master );
	FD_SET( base_server_socket, &master );

	while ( is_running ) {
		fd_set copy = master;
		int count = select( 0, &copy, nullptr, nullptr, nullptr );
		for ( int i = 0; i < count; i++ ) {
			SOCKET csock = copy.fd_array[ i ];
			if ( csock == base_server_socket ) {
				SOCKET connected_client = accept( base_server_socket, 0, 0 );		
				auto connected_client_socket = c_socket( connected_client, e_socket_type::client );			
				if ( on_client_connected( connected_client_socket ) ) {
					container.on_connected( connected_client_socket );

					FD_SET( connected_client, &master );

					on_client_connection_success( connected_client_socket );
				}
				else { 
					closesocket( connected_client ); 
					connected_client = 0;
				}
			}
			else {
				ZeroMemory( read_buffer, buf_size );
				int bytes_received = recv( csock, ( char* )read_buffer, buf_size, 0 );
				if ( bytes_received <= 0 ) {
					on_client_disconnected( csock );
					container.on_disconnected( csock );

					FD_CLR( csock, &master );
				}
				else {
					packet_container.on_data_received( csock, net_bridge::c_array( ( int8_t* )read_buffer, bytes_received ) );
				}
			}
		}
	}
}

void net_bridge::c_tcp_server::send_to_client( c_socket client, net_bridge::c_array arr )
{
	packets_to_send.push_back( { client, arr } );
}