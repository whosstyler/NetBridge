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
	// Loop while the server is running
	while ( is_running ) {
		// Get all packets received since the last call to this function
		auto packets = packet_container.get_packets( );

		// Iterate over each client's packet queue
		for ( auto& fclient : packets ) {
			auto& q = fclient.second;

			// Process each packet in the queue
			while ( !q->empty( ) ) {
				net_bridge::c_packet& packet = q->front( );

				// If the packet is not yet completed, break out of the loop and wait for more data
				if ( packet.info.state != e_packet_state::completed )
					break;

				// If the packet hasn't been handled yet, mark it as handled and process it
				if ( !packet.info.is_handled ) {
					packet.info.is_handled = true;

					// Get the socket for this client
					auto client_sock = container.get_client( fclient.first );

					// If the socket is invalid, clear the packet queue and move on to the next client
					if ( client_sock.get_socket( ) == 0 ) {
						Log( "%s | failed to get client_socket [%llu]", __FUNCTION__, fclient.first );
						(void)q->empty( );
						continue;
					}

					// Handle the packet and remove it from the queue
					Log( "Handling Packet [%llx]", q->size( ) );
					on_handle_packet( client_sock, packet );
					q->pop( );
				}
			}
		}

		// Wait a short time before checking for more packets
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
	// set the read buffer size to 1MB
	size_t read_buffer_size = 1024 * 1024;

	// allocate memory for the read buffer
	BYTE* read_buffer = new BYTE[ read_buffer_size ];

	// create a file descriptor set and add the server socket to it
	fd_set master;
	auto base_server_socket = server_socket->get_socket( );
	FD_ZERO( &master );
	FD_SET( base_server_socket, &master );

	// loop while the server is running
	while ( is_running ) {

		// create a copy of the file descriptor set and wait for an event
		fd_set copy = master;
		int count = select( 0, &copy, nullptr, nullptr, nullptr );

		// iterate over each file descriptor that has an event
		for ( int i = 0; i < count; i++ ) {
			SOCKET csock = copy.fd_array[ i ];

			// if the event is from the server socket, accept the client's connection
			if ( csock == base_server_socket ) {
				SOCKET connected_client = accept( base_server_socket, 0, 0 );
				auto connected_client_socket = c_socket( connected_client, e_socket_type::client );

				// if the client is accepted, add the client's socket to the file descriptor set
				if ( on_client_connected( connected_client_socket ) ) {
					container.on_connected( connected_client_socket );
					FD_SET( connected_client, &master );
					on_client_connection_success( connected_client_socket );
				}
				else {
					// if the client is not accepted, close the client's socket
					closesocket( connected_client );
					connected_client = 0;
				}
			}
			// if the event is from a client socket, receive the data
			else {
				// clear the read buffer and receive the data
				ZeroMemory( read_buffer, read_buffer_size );
				int bytes_received = recv( csock, ( char* )read_buffer, read_buffer_size, 0 );

				// if the client has disconnected, remove the client's socket from the file descriptor set
				if ( bytes_received <= 0 ) {
					on_client_disconnected( csock );
					container.on_disconnected( csock );
					FD_CLR( csock, &master );
				}
				// otherwise, pass the received data to the packet container for processing
				else 
					packet_container.on_data_received( csock, net_bridge::c_array( ( int8_t* )read_buffer, bytes_received ) );
			}
		}
	}
}

void net_bridge::c_tcp_server::send_to_client( c_socket client, net_bridge::c_array arr )
{
	packets_to_send.push_back( { client, arr } );
}