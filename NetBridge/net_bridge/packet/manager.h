#pragma once
#include "../../net_bridge.h"

namespace net_bridge
{
	class c_scalarlength {
	public:
		inline c_scalarlength( ) {}
		inline c_scalarlength( uint64_t length )
		{
			scalar_length = length;

			fix_index( );
		}

		void fix_index( );

		uint8_t scalar_index = 0;
		uint64_t scalar_length = 0;
	};

	class c_array {
	public:

		inline c_array( ) {}

		inline c_array( const c_array& obj )
		{
			init( ( int8_t* )obj.buffer.data( ), obj.buffer_length );
		}

		inline c_array( int8_t* in_buffer, std::uint64_t length )
		{
			init( in_buffer, length );
		}

		void init( int8_t* in_buffer, std::uint64_t length ) {
			buffer_length = length;

			buffer.insert( buffer.end( ), in_buffer, in_buffer + length );
		}


	public:
		template<class obj_type>
		void write( obj_type obj ) {
			//std::lock_guard<std::mutex> guard(mtx);

			auto obj_size = sizeof( obj_type );

			int8_t* bytes = new int8_t[ obj_size ];

			memcpy( bytes, &obj, obj_size );

			buffer.insert( buffer.end( ), bytes, bytes + obj_size );

			delete[ ] bytes;
			bytes = nullptr;

			write_offset += obj_size;

			buffer_length += obj_size;
		}

		template<class obj_type>
		bool read( obj_type& obj ) {
			//std::lock_guard<std::mutex> guard(mtx);
			auto obj_size = sizeof( obj_type );
			if ( obj_size + read_offset > buffer_length ) return false;

			auto raw_buffer = buffer.data( );

			obj = *( obj_type* )( raw_buffer + read_offset );

			read_offset += obj_size;

			return true;
		}

		bool read_string( std::string& obj ) {
			bool result = false;
			uint16_t string_size = 0;

			if ( read<uint16_t>( string_size ) ) {

				bool failed = false;
				for ( int i = 0; i < string_size; i++ )
				{
					char c = 0;
					if ( !read<char>( c ) ) {
						failed = true;
						break;
					}

					obj += c;
				}

				result = !failed;
			}

			return result;
		}

		void write_string( std::string obj ) {

			auto size = obj.length( );
			write<uint16_t>( size );

			write( ( int8_t* )obj.c_str( ), size );
		}

		bool read( int8_t* out_buffer, size_t size ) {
			//std::lock_guard<std::mutex> guard(mtx);
			size_t read_size = size;

			if ( read_size == -1 )
				read_size = buffer_length - read_offset;

			if ( read_size > buffer_length || !read_size ) return false;


			auto raw_buffer = buffer.data( );

			memcpy( out_buffer, ( raw_buffer + read_offset ), read_size );

			read_offset += read_size;

			return true;
		}

		void write( int8_t* write_buffer, size_t size ) {
			//	std::lock_guard<std::mutex> guard(mtx);
			buffer.insert( buffer.end( ), write_buffer, write_buffer + size );

			write_offset += size;

			buffer_length += size;
		}

		bool read_scalar( c_scalarlength& obj ) {
			bool result = false;
			if ( read<uint8_t>( obj.scalar_index ) ) {


				switch ( obj.scalar_index ) {
				case 1:
				{
					uint8_t byte_length_scalar = 0;
					result = read<uint8_t>( byte_length_scalar );

					obj.scalar_length = byte_length_scalar;

					break;
				}

				case 2:
				{
					uint16_t byte_length_scalar = 0;
					result = read<uint16_t>( byte_length_scalar );

					obj.scalar_length = byte_length_scalar;

					break;
				}
				case 4:
				{
					uint32_t byte_length_scalar = 0;
					result = read<uint32_t>( byte_length_scalar );

					obj.scalar_length = byte_length_scalar;

					break;
				}

				case 8:
				{
					uint64_t byte_length_scalar = 0;
					result = read<uint64_t>( byte_length_scalar );

					obj.scalar_length = byte_length_scalar;

					break;
				}
				}

			}

			return result;
		}

		void write_scalar( c_scalarlength obj ) {
			write_scalar( obj.scalar_index, obj.scalar_length );
		}

		void write_scalar( uint64_t length ) {
			write_scalar( c_scalarlength( length ) );
		}

		void write_scalar( uint8_t index, uint64_t length ) {

			write<uint8_t>( index );
			switch ( index ) {
			case 1:
			{
				write<uint8_t>( length );
				break;
			}
			case 2:
			{
				write<uint16_t>( length );
				break;
			}
			case 4:
			{
				write<uint32_t>( length );
				break;
			}

			case 8:
			{
				write<uint64_t>( length );
				break;
			}
			}
		}


		void remove_bytes( uint64_t x ) {

			buffer.erase( buffer.begin( ), buffer.begin( ) + x );


			buffer_length -= x;

			write_offset = buffer_length;
			read_offset = 0;
		}

	public:
		uint64_t get_read_offset( ) { return read_offset; }

		uint64_t get_write_offset( ) { return write_offset; }

		uint64_t get_buffer_length( ) { return buffer_length; }

		int8_t* get_buffer( ) { return buffer.data( ); };

		BYTE* get_bytes( ) { return ( BYTE* )buffer.data( ); };

		uint64_t bytes_remaining( ) { return get_buffer_length( ) - get_read_offset( ); };

	private:
		std::vector<int8_t> buffer;

		uint64_t read_offset = 0;
		uint64_t write_offset = 0;
		uint64_t buffer_length = 0;

	};
}