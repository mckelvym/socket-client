#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdio>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include "config.H"
#include <libgen.h>

using namespace std;

// ---------------------------------------------------------------------------
char const *progname;
config_t *cfg;

bool setup_connection ();
void quit_app ();
void signal_handler (int signal);
void print_usage_and_exit ();
void message (string type, string str);
// ---------------------------------------------------------------------------

int main(int argc, char **args)
{
	int opt;
	bool usage_error = false;

	// Set default values
	progname = basename (args[0]);
	signal (SIGINT, signal_handler);
	signal (SIGKILL, signal_handler);
	cfg = new config_t ();

	while ((opt = getopt (argc, args, "b:h:p:s:v?")) != -1 && !usage_error)
	{
		switch (opt)
		{
		case 'b':
			if (sscanf (optarg, "%u", &cfg->m_buffer_size) != 1)
				usage_error = true;
			break;
		case 'h':
			cfg->m_host = optarg;
			break;
		case 'p':
			if (sscanf (optarg, "%u", &cfg->m_server_port) != 1)
				usage_error = true;
			break;
		case 's':
			if (sscanf (optarg, "%u", &cfg->m_sleep_amount) != 1)
				usage_error = true;
			break;
		case 'v':
			cfg->m_verbose = true;
			break;
		case '?':
			usage_error = true;
			break;
		}
	}
	if (usage_error)
		print_usage_and_exit ();

	// ----------------------------------------


	// look up host's IP address
	cfg->m_hostentry = gethostbyname (cfg->m_host.c_str ());
	if (!cfg->m_hostentry)
	{
		message ("fatal error", "gethostbyname() failed.");
		quit_app ();
	}

	while (!setup_connection ())
	{
		message ("warning", "couldn't connect. Sleeping before trying again.");
		usleep (cfg->m_sleep_amount);
	}
	// connection is now established, begin reading from server

	while (true)
	{
		char * buffer = new char[cfg->m_buffer_size];

		// indicate to server that it is ok to write data
		char ack[2] = "1";
		send (cfg->m_socket_id, ack, strlen (ack)+1, 0);

		message ("info", "reading data.");
		// read from socket
		cfg->m_data = read(cfg->m_socket_id, &buffer, cfg->m_buffer_size);

		if (cfg->m_data <= 0){ // check for end of file
			message ("warning", "disconnected from server.");
			usleep (cfg->m_sleep_amount);

			// close current connection
			shutdown (cfg->m_socket_id, SHUT_RDWR);

			message ("info", "trying to reconnect to server.");
			while (!setup_connection ())
			{
				message ("warning", "couldn't connect. Sleeping before trying again.");
				usleep (cfg->m_sleep_amount);
			}
			message ("info", "reconnect ok.");
		}
		else
			write(1, &buffer, cfg->m_data);		/* write to standard output */
	}
}

bool setup_connection ()
{
	// allocate socket
	cfg->m_socket_id =
		socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (cfg->m_socket_id < 0)
	{
		message ("error", "socket allocation failed.");
		quit_app ();
		return false;
	}

	memset (&cfg->m_channel, 0, sizeof (cfg->m_channel));
	cfg->m_channel.sin_family= AF_INET;
	memcpy (&cfg->m_channel.sin_addr.s_addr, cfg->m_hostentry->h_addr,
		cfg->m_hostentry->h_length);
	cfg->m_channel.sin_port = htons (cfg->m_server_port);

	// connect to host
	cfg->m_connection_id =
		connect (cfg->m_socket_id,
			 (struct sockaddr *) &cfg->m_channel,
			 sizeof (cfg->m_channel));
	if (cfg->m_connection_id < 0)
	{
		message ("error", "connection failed.");
		quit_app ();
		return false;
	}
	return true;
}

void quit_app ()
{
	message ("info", "closing socket and shutting down.\n");

	// indicate to server to stop sending data
	char ack[2] = "0";
	send (cfg->m_socket_id, ack, strlen (ack)+1, 0);
	shutdown (cfg->m_socket_id, SHUT_RDWR);
	exit (0);
}

void signal_handler (int signal)
{
	printf ("%s: received signal %d, Exiting.\n",
		progname, signal);
	quit_app ();
}

void print_usage_and_exit ()
{
	printf ("\nUsage: %s <options>\n", progname);
	printf ("\t[-b #]\t\t Buffer transfer size [%u].\n", cfg->m_buffer_size);
	printf ("\t[-h str]\t Hostname of server [%s].\n", cfg->m_host.c_str ());
	printf ("\t[-p #]\t\t Port to use [%u].\n", cfg->m_server_port);
	printf ("\t[-s #]\t\t Sleep before reconnect attempt [%u].\n", cfg->m_sleep_amount);
	printf ("\t[-v]\t\t Turn on verbosity.\n");
	printf ("\n\n");
	exit (0);
}

void message (string type, string str)
{
	if (cfg->m_verbose)
	{
		fprintf (stderr, "%s: %s: %s\n", progname,
			 type.c_str (),
			 str.c_str ());
	}
}
