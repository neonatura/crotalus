Crotalus (Boa) is a single-tasking HTTP server.  That means that unlike
traditional web servers, it does not fork for each incoming connection,
nor does it fork many copies of itself to handle multiple connections.
It internally multiplexes all of the ongoing HTTP connections, and
forks only for CGI programs (which must be separate processes),
automatic directory generation, and automatic file gunzipping.
The exception is PHP scripts with are handled on seperate threads.

Modifications to the Crotalus web server that improve its speed, security,
robustness, and portability, are eagerly sought.  Other features may be
added if they can be achieved without hurting the primary goals.
