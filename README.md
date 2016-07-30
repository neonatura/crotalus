Crotalus is a light-weight HTTP server.  

Crotalus internally multiplexes all of the ongoing HTTP connections. CGI and PHP programs are ran in a process pool.

Crotalus runs CGI programs as separate processes, performs automatic directory generation, and automatic file gunzipping.

The Crotalus web server has the primary ambitions of improved speed, security,
robustness.

Purpose
=======

This is crotalus, a high performance web server for "unix compiler" computers. This project strives to combine the benefits of a highly stream-lined single-threaded web server and common HTTP standards such as SSL and inline PHP interpretting.

This package is being provided in order to make use of the web server provided with the libshare library suite (github.com/neonatura/share), but does not require having to integrate with the libshare functionality.


Install Instructions
====================

git clone https://github.com/neonatura/crotalus/
cd crotalus
mkdir build; cd build
../configure
make
make install


Run "configure --help" in order to see additional options relating to limiting external dependencies.

The file "/etc/crotalus/crotalus.conf" will automatically be generated which contains descriptions of all the options available to be set.


Upcoming Features
=================

None of the following items have been implemented;

* SSL (https) support.
* OpenAuth API support.
* win32/gnulib compatibility support
* 'make install' support for ubuntu



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


