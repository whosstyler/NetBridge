#include "net_packet.h"

void net_bridge::c_packet_container::on_data_received( std::uint64_t sock, net_bridge::c_array arr )
{
	net_bridge::c_array& buffer = received_data[ sock ]; // Reference to buffer for this socket
	buffer.write( arr.get_buffer( ), arr.get_buffer_length( ) ); // Write data from arr into the buffer

    if ( !received_packets[ sock ] ) // Check if there are no packets for this socket
        received_packets[ sock ] = new std::queue<net_bridge::c_packet>; // Create a new packet queue

    while ( true ) { // Loop until we break out
        // Check if the deque is empty or the last packet is complete
        if ( received_packets[ sock ]->empty( ) || received_packets[ sock ]->back( ).info.state == e_packet_state::completed ) 
            received_packets[ sock ]->emplace( );  // Add a new packet to the deque
   
        net_bridge::c_packet& awaiting_packet = received_packets[ sock ]->back( );  // Reference to the last packet in the deque

        if ( !awaiting_packet.info.is_header_valid ) { // Check if header is not yet valid
            if ( buffer.bytes_remaining( ) >= 10 ) { // Check if there are enough bytes for header
                buffer.read<uint16_t>( awaiting_packet.header.magic_id ); // Read the magic id
                buffer.read<uint64_t>( awaiting_packet.header.body_length ); // Read the body length

                awaiting_packet.info.is_header_valid = true; // Mark the header as valid
            }
            else break; // If there are not enough bytes for header, break out
        }

        if ( awaiting_packet.info.is_header_valid && !awaiting_packet.info.is_body_valid ) { // Check if header is valid and body is not yet valid
            if ( buffer.bytes_remaining( ) >= awaiting_packet.header.body_length ) { // Check if there are enough bytes for the body
                uint8_t opcode = 0;
                buffer.read<uint8_t>( opcode ); // Read the opcode

                awaiting_packet.body.opcode = static_cast< e_packet_core_opcode >( opcode ); // Set the opcode

                uint64_t message_size = awaiting_packet.header.body_length - 1; // Calculate the message size

                if ( message_size ) { // If the message size is greater than zero
                    int8_t* message_buffer = new int8_t[ message_size ]; // Allocate memory for the message buffer

                    buffer.read( message_buffer, message_size ); // Read the message buffer from the buffer

                    auto bmessage = c_array( message_buffer, message_size ); // Create a c_array for the message
                    awaiting_packet.body.message.init( message_buffer, message_size ); // Initialize the message buffer in the packet

                    delete[ ] message_buffer; // Deallocate the message buffer

                    message_buffer = nullptr; // Set the pointer to null
                }

                awaiting_packet.info.state = e_packet_state::completed; // Mark the packet as completed
                awaiting_packet.info.is_body_valid = true; // Mark the body as valid

                buffer.remove_bytes( awaiting_packet.header.body_length + 10 ); // Remove the packet bytes from the buffer
            }
            else break; // If there are not enough bytes for the body, break out
        }

        if ( buffer.bytes_remaining( ) < 10 ) break; // If there are not enough bytes for another header, break out
    }
}
