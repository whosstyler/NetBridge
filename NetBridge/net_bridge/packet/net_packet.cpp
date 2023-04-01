#include "net_packet.h"

void net_bridge::c_packet_container::on_data_received( std::uint64_t sock, net_bridge::c_array arr )
{
    net_bridge::c_array& buffer = received_data[ sock ];

    buffer.write( arr.get_buffer( ), arr.get_buffer_length( ) );

    if ( !received_packets[ sock ] )
		received_packets[ sock ] = new std::deque<net_bridge::c_packet>( );

    while ( 1 ) {
        if ( received_packets[ sock ]->empty( ) || received_packets[ sock ]->back( ).info.state == e_packet_state::completed ) 
            received_packets[ sock ]->emplace_back( );

        net_bridge::c_packet& awaiting_packet = received_packets[ sock ]->back( );

        const int PACKET_HEADER_SIZE = 10;
        if ( !awaiting_packet.info.is_header_valid && buffer.bytes_remaining( ) >= PACKET_HEADER_SIZE ) {
            buffer.read<uint16_t>( awaiting_packet.header.magic_id );
            buffer.read<uint64_t>( awaiting_packet.header.body_length );
            awaiting_packet.info.is_header_valid = true;
        }

        if ( awaiting_packet.info.is_header_valid && !awaiting_packet.info.is_body_valid ) {
            if ( buffer.bytes_remaining( ) < awaiting_packet.header.body_length )
                break;

            uint8_t opcode = 0;
            buffer.read<uint8_t>( opcode );
            awaiting_packet.body.opcode = ( e_packet_core_opcode )opcode;
           
            uint64_t message_size = awaiting_packet.header.body_length - 1;
            if ( message_size ) {
                int8_t* message_buffer = new int8_t[ message_size ];
                buffer.read( message_buffer, message_size );

                auto bmessage = c_array( message_buffer, message_size );
                awaiting_packet.body.message.init( message_buffer, message_size );
                delete[ ] message_buffer;
                message_buffer = nullptr;
            }

            awaiting_packet.info.state = e_packet_state::completed;
            awaiting_packet.info.is_body_valid = true;
            buffer.remove_bytes( awaiting_packet.header.body_length + 10 );
        }
        if ( buffer.bytes_remaining( ) < 10 ) 
            break;
    }
}
