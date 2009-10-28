#include <cassert>
#include "config.H"

config_t::config_t ()
{
	m_verbose = false;
	m_host = "localhost";
	m_server_port = 8800;
	m_buffer_size = 4096;
	m_sleep_amount = 1000;
	m_socket_id = -1;
	m_connection_id = -1;
	m_data = -1;
}

