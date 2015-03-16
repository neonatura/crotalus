Crotalus is a single-tasking HTTP server.  That means that unlike
traditional web servers, it does not fork for each incoming connection,
nor does it fork many copies of itself to handle multiple connections.
It internally multiplexes all of the ongoing HTTP connections, and
runs CGI programs as separate processes, performs automatic directory 
generation, and automatic file gunzipping.

Modifications to the Crotalus web server that improve its speed, security,
robustness, and portability, are eagerly sought.  Other features may be
added if they can be achieved without hurting the primary goals.

Purpose
=======

This is crotalus, a high performance web server for 'unix compiler' computers. This project strives to combine the benefits of a highly stream-lined single-threaded web server and common HTTP standards such as SSL and inline PHP interpretting.

This package is being provided in order to make use of the web server provided with the libshare library suite (github.com/neonatura/share), but does not require having to integrate with the libshare functionality.


Upcoming Features
=================

None of the following items have been implemented;

* nanohttp feature support (including ssl, registered callbacks, and misc auth)
* inline PHP for files ending in ".php" and executable
* win32/gnulib compatibility support
* 'make install' support for ubuntu


Performance Specifications
==========================

The secret behind it's high-performance is a single-threaded monolithic design with all focus going towards the web engine. This contrasts with common web servers, such as apache, which utilizes multiple processes or threads (based on config) in order to achieve high-volume web page serving and requires a much larger frame-work in order to accomodate common modules and standards. A real world comparison may be of the likes of a motorcycle compared to a freight truck. The freight truck will be able to deliver a much wider array of content, but when a lighter load is delivered the motorcycle will always be able to out-perform.


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


