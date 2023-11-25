# Socket Client

Creates a socket connection on the configured port to the specified hostname and reads until the connection is closed.

See also: [https://github.com/mckelvym/socket-server](https://github.com/mckelvym/socket-server)

## Build

```bash
$ make
g++ -I/usr/include -L/usr/lib -o config.o -c config.C
g++ -I/usr/include -L/usr/lib -o client.o -c client.C
g++  -o client config.o client.o 
```

## Usage

```bash
$ ./client -?

Usage: client <options>
	[-b #]		 Buffer transfer size [4096].
	[-h str]	 Hostname of server [localhost].
	[-p #]		 Port to use [8800].
	[-s #]		 Sleep before reconnect attempt [1000].
	[-v]		 Turn on verbosity.
```

## Run

```bash
$ ./client -p 8801 -v
client: info: reading data.
??#W??Ó??
        cʮ?W?N_]t??[?B?'M?t??!???	??!d
```
