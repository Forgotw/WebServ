
BIND(2)                     BSD System Calls Manual                    BIND(2)

NNAAMMEE
     bbiinndd -- bind a name to a socket

SSYYNNOOPPSSIISS
     ##iinncclluuddee <<ssyyss//ssoocckkeett..hh>>

     _i_n_t
     bbiinndd(_i_n_t _s_o_c_k_e_t, _c_o_n_s_t _s_t_r_u_c_t _s_o_c_k_a_d_d_r _*_a_d_d_r_e_s_s, _s_o_c_k_l_e_n___t _a_d_d_r_e_s_s___l_e_n);

DDEESSCCRRIIPPTTIIOONN
     bbiinndd() assigns a name to an unnamed socket.  When a socket is created
     with socket(2) it exists in a name space (address family) but has no name
     assigned.  bbiinndd() requests that _a_d_d_r_e_s_s be assigned to the socket.

NNOOTTEESS
     Binding a name in the UNIX domain creates a socket in the file system
     that must be deleted by the caller when it is no longer needed (using
     unlink(2)).

     The rules used in name binding vary between communication domains.  Con-
     sult the manual entries in section 4 for detailed information.

RREETTUURRNN VVAALLUUEESS
     Upon successful completion, a value of 0 is returned.  Otherwise, a value
     of -1 is returned and the global integer variable _e_r_r_n_o is set to indi-
     cate the error.

EERRRROORRSS
     The bbiinndd() system call will fail if:

     [EACCES]           The requested address is protected, and the current
                        user has inadequate permission to access it.

     [EADDRINUSE]       The specified address is already in use.

     [EADDRNOTAVAIL]    The specified address is not available from the local
                        machine.

     [EAFNOSUPPORT]     _a_d_d_r_e_s_s is not valid for the address family of _s_o_c_k_e_t.

     [EBADF]            _s_o_c_k_e_t is not a valid file descriptor.

     [EDESTADDRREQ]     _s_o_c_k_e_t is a null pointer.

     [EFAULT]           The _a_d_d_r_e_s_s parameter is not in a valid part of the
                        user address space.

     [EINVAL]           _s_o_c_k_e_t is already bound to an address and the protocol
                        does not support binding to a new address.  Alterna-
                        tively, _s_o_c_k_e_t may have been shut down.

     [ENOTSOCK]         _s_o_c_k_e_t does not refer to a socket.

     [EOPNOTSUPP]       _s_o_c_k_e_t is not of a type that can be bound to an
                        address.

     The following errors are specific to binding names in the UNIX domain.

     [EACCES]           A component of the path prefix does not allow search-
                        ing or the node's parent directory denies write per-
                        mission.

     [EEXIST]           A file already exists at the pathname.  unlink(2) it
                        first.

     [EIO]              An I/O error occurred while making the directory entry
                        or allocating the inode.

     [EISDIR]           An empty pathname was specified.

     [ELOOP]            Too many symbolic links were encountered in translat-
                        ing the pathname.  This is taken to be indicative of a
                        looping symbolic link.

     [ENAMETOOLONG]     A component of a pathname exceeded {NAME_MAX} charac-
                        ters, or an entire path name exceeded {PATH_MAX} char-
                        acters.

     [ENOENT]           A component of the path name does not refer to an
                        existing file.

     [ENOTDIR]          A component of the path prefix is not a directory.

     [EROFS]            The name would reside on a read-only file system.

LLEEGGAACCYY SSYYNNOOPPSSIISS
     ##iinncclluuddee <<ssyyss//ttyyppeess..hh>>
     ##iinncclluuddee <<ssyyss//ssoocckkeett..hh>>

     The include file <_s_y_s_/_t_y_p_e_s_._h> is necessary.

SSEEEE AALLSSOO
     connect(2), connectx(2), getsockname(2), listen(2), socket(2), compat(5)

HHIISSTTOORRYY
     The bbiinndd() function call appeared in 4.2BSD.

4.2 Berkeley Distribution       March 18, 2015       4.2 Berkeley Distribution
