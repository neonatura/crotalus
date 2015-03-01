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

Purpose
=======

This is crotalus, a high performance web server for linux or windows computers. This project strives to combine the benefits of a highly stream-lined single-threaded web server and common HTTP standards such as SSL and inline PHP interpretting.

This package is being provided in order to make use of the web server provided with the libshare library suite (github.com/neonatura/share), but does not require having to integrate with the libshare functionality.


Upcoming Features
=================

None of the following items have been implemented;

* 'default' directory support (retracts to parent dir [and so on] if url path doesnt exist)
* nanohttp feature support (including ssl, registered callbacks, and misc auth)
* inline PHP for files ending in ".php" and executable
* doxygen HTML & man-page documentation (incl. extended source commenting)
* win32/gnulib compatibility support
* 'make install' (support incl. /etc/rc.d/ sub-system & w/e ubuntu uses)


Performance Specifications
==========================

The crotalus webserver can handle executing more than a hundred cgi scripts per second, and is capable of much higher performance with static HTML pages. By comparison, a apache web server running a medium-sized PHP application can take up over 100 processes to render over one million pages per day (11/sec). 

  Doc Note: A more 'apple to apple' comparison is needed here.

The secret behind it's high-performance is a single-threaded monolithic design with all focus going towards the web engine. This contrasts with common web servers, such as apache, which utilizes multiple processes or threads (based on config) in order to achieve high-volume web page serving and requires a much larger frame-work in order to accomodate common modules and standards. A real world comparison may be of the likes of a motorcycle compared to a freight truck. The freight truck will be able to deliver a much wider array of content, but when a lighter load is delivered the motorcycle will always be able to out-perform.

There are limits, though, to this model. Due to the monolithic (synchronous) nature of the web engine, each page rendered by cgi (PHP or otherwise) must be written in a manner (such as ajax) which ensures a small life-span. This is because each page is rendered before the next is handled. 


Background History
==================

Crotalus is a genus of venomous pit vipers [found only in the Americas] derived from the Greek word krotalon, which means "rattle" or "castanet", and refers to the rattle on the end of the tail.

This package is based off of Boa, a high performance web server for 
Unix-alike computers, covered by the Gnu General Public License. 
Derived from boa version 0.94, released January 2000.  

Boa was created in 1991 by Paul Phillips <paulp@go2net.com>.  It is now being
maintained and enhanced by Larry Doolittle <ldoolitt@boa.org>
and Jon Nelson <jnelson@boa.org>.

For more information (including installation instructions) examine
the file docs/boa.txt or docs/boa.dvi, point your web browser to docs/boa.html,
or visit the Boa homepage at

        http://www.boa.org/


