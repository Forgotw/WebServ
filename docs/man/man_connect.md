
CONNECT(2)                  BSD System Calls Manual                 CONNECT(2)

NNAAMMEE
     ccoonnnneecctt -- initiate a connection on a socket

SSYYNNOOPPSSIISS
     ##iinncclluuddee <<ssyyss//ttyyppeess..hh>>
     ##iinncclluuddee <<ssyyss//ssoocckkeett..hh>>

     _i_n_t
     ccoonnnneecctt(_i_n_t _s_o_c_k_e_t, _c_o_n_s_t _s_t_r_u_c_t _s_o_c_k_a_d_d_r _*_a_d_d_r_e_s_s,
         _s_o_c_k_l_e_n___t _a_d_d_r_e_s_s___l_e_n);

DDEESSCCRRIIPPTTIIOONN
     The parameter _s_o_c_k_e_t is a socket.  If it is of type SOCK_DGRAM, this call
     specifies the peer with which the socket is to be associated; this
     address is that to which datagrams are to be sent, and the only address
     from which datagrams are to be received.  If the socket is of type
     SOCK_STREAM, this call attempts to make a connection to another socket.
     The other socket is specified by _a_d_d_r_e_s_s, which is an address in the com-
     munications space of the socket.

     Each communications space interprets the _a_d_d_r_e_s_s parameter in its own
     way.  Generally, stream sockets may successfully ccoonnnneecctt() only once;
     datagram sockets may use ccoonnnneecctt() multiple times to change their associ-
     ation.  Datagram sockets may dissolve the association by calling
     disconnectx(2), or by connecting to an invalid address, such as a null
     address or an address with the address family set to AF_UNSPEC (the error
     EAFNOSUPPORT will be harmlessly returned).

RREETTUURRNN VVAALLUUEESS
     Upon successful completion, a value of 0 is returned.  Otherwise, a value
     of -1 is returned and the global integer variable _e_r_r_n_o is set to indi-
     cate the error.

EERRRROORRSS
     The ccoonnnneecctt() system call will fail if:

     [EACCES]           The destination address is a broadcast address and the
                        socket option SO_BROADCAST is not set.

     [EADDRINUSE]       The address is already in use.

     [EADDRNOTAVAIL]    The specified address is not available on this
                        machine.

     [EAFNOSUPPORT]     Addresses in the specified address family cannot be
                        used with this socket.

     [EALREADY]         The socket is non-blocking and a previous connection
                        attempt has not yet been completed.

     [EBADF]            _s_o_c_k_e_t is not a valid descriptor.

     [ECONNREFUSED]     The attempt to connect was ignored (because the target
                        is not listening for connections) or explicitly
                        rejected.

     [EFAULT]           The _a_d_d_r_e_s_s parameter specifies an area outside the
                        process address space.

     [EHOSTUNREACH]     The target host cannot be reached (e.g., down, discon-
                        nected).

     [EINPROGRESS]      The socket is non-blocking and the connection cannot
                        be completed immediately.  It is possible to select(2)
                        for completion by selecting the socket for writing.

     [EINTR]            Its execution was interrupted by a signal.

     [EINVAL]           An invalid argument was detected (e.g., _a_d_d_r_e_s_s___l_e_n is
                        not valid for the address family, the specified
                        address family is invalid).

     [EISCONN]          The socket is already connected.

     [ENETDOWN]         The local network interface is not functioning.

     [ENETUNREACH]      The network isn't reachable from this host.

     [ENOBUFS]          The system call was unable to allocate a needed memory
                        buffer.

     [ENOTSOCK]         _s_o_c_k_e_t is not a file descriptor for a socket.

     [EOPNOTSUPP]       Because _s_o_c_k_e_t is listening, no connection is allowed.

     [EPROTOTYPE]       _a_d_d_r_e_s_s has a different type than the socket that is
                        bound to the specified peer address.

     [ETIMEDOUT]        Connection establishment timed out without establish-
                        ing a connection.

     [ECONNRESET]       Remote host reset the connection request.

     The following errors are specific to connecting names in the UNIX domain.
     These errors may not apply in future versions of the UNIX IPC domain.

     [EACCES]           Search permission is denied for a component of the
                        path prefix.

     [EACCES]           Write access to the named socket is denied.

     [EIO]              An I/O error occurred while reading from or writing to
                        the file system.

     [ELOOP]            Too many symbolic links were encountered in translat-
                        ing the pathname.  This is taken to be indicative of a
                        looping symbolic link.

     [ENAMETOOLONG]     A component of a pathname exceeded {NAME_MAX} charac-
                        ters, or an entire path name exceeded {PATH_MAX} char-
                        acters.

     [ENOENT]           The named socket does not exist.

     [ENOTDIR]          A component of the path prefix is not a directory.

LLEEGGAACCYY SSYYNNOOPPSSIISS
     ##iinncclluuddee <<ssyyss//ttyyppeess..hh>>
     ##iinncclluuddee <<ssyyss//ssoocckkeett..hh>>

     The include file <_s_y_s_/_t_y_p_e_s_._h> is necessary.

SSEEEE AALLSSOO
     accept(2), connectx(2), disconnectx(2), getsockname(2), select(2),
     socket(2), compat(5)

HHIISSTTOORRYY
     The ccoonnnneecctt() function call appeared in 4.2BSD.

4.2 Berkeley Distribution       March 18, 2015       4.2 Berkeley Distribution
