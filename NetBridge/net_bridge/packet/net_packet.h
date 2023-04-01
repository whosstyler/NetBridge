#pragma once
#include "../../net_bridge.h"
#include "manager.h"

namespace net_bridge
{
    enum class e_packet_core_opcode : uint8_t
    {
        none,
        //client -> C2S
        notify_csnencryptedsessionkey_message,
        //server -> S2C
        notify_serverconnectionhint_message,
        notify_serverconnection_success,
        //common -> C2S | S2C
        rmi,
        encrypyted_reliable
    };

    enum class e_packet_state {
        none,
        waiting_header,
        waiting_body,
        completed
    };

    class c_packet_info {
    public:
        e_packet_state state = e_packet_state::waiting_body;

        bool is_header_valid = false;

        bool is_body_valid = false;

        bool is_handled = false;
    };

    class c_header {
    public:
        uint16_t magic_id = 77;
        uint64_t body_length;
    };

    class c_body {
    public:
        e_packet_core_opcode opcode = e_packet_core_opcode::none;

        c_array message;
    };
    
    class c_packet {
    public:
        inline c_packet( ) {}

        c_packet_info info;
        c_header header;
        c_body body;

        c_array write( ) {
            c_array arr;
            arr.write<uint16_t>( header.magic_id );
            arr.write<uint64_t>( header.body_length );
            arr.write( ( uint8_t )body.opcode );
            arr.write( body.message.get_buffer( ), body.message.get_buffer_length( ) );
            return arr;
        }

        void fix_size( ) {
            header.body_length = 1 + body.message.get_buffer_length( );
        }
    };

    class c_packet_container {
    public:
        inline c_packet_container( ) {}
        
        void on_data_received( std::uint64_t sock, c_array arr );
        std::map<std::uint64_t, std::deque<c_packet>*> get_packets( ) { return received_packets; }

        void new_client( uint64_t c ) {
            received_packets[ c ] = new std::deque<c_packet>( );
        }
    private:
        std::map<std::uint64_t, std::deque<c_packet>*> received_packets;
        std::map<std::uint64_t, c_array> received_data;
    };
};

