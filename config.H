#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <netinet/in.h>

using namespace std;

class config_t
{
public:
	config_t ();

	bool m_verbose;

	// remote host, either ip or hostname
	string m_host;

	// client and server must agree on port
	unsigned m_server_port;

	// block transfer size
	unsigned m_buffer_size;

	// sleep amount before trying reconnect
	unsigned m_sleep_amount;

	// used to represent an entry in the hosts database,
	// info about server
	struct hostent *m_hostentry;

	// a local or remote endpoint address that a socket connects to,
	// holds IP address
	struct sockaddr_in m_channel;

	int m_socket_id;
	int m_connection_id;
	int m_data;

private:
};

#endif
