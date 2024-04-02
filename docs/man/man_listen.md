
LISTEN(2)                   BSD System Calls Manual                  LISTEN(2)

NNAAMMEE
     lliisstteenn -- listen for connections on a socket

SSYYNNOOPPSSIISS
     ##iinncclluuddee <<ssyyss//ssoocckkeett..hh>>

     _i_n_t
     lliisstteenn(_i_n_t _s_o_c_k_e_t, _i_n_t _b_a_c_k_l_o_g);

DDEESSCCRRIIPPTTIIOONN
     Creation of socket-based connections requires several operations.  First,
     a socket is created with socket(2).  Next, a willingness to accept incom-
     ing connections and a queue limit for incoming connections are specified
     with lliisstteenn().  Finally, the connections are accepted with accept(2).
     The lliisstteenn() call applies only to sockets of type SOCK_STREAM.

     The _b_a_c_k_l_o_g parameter defines the maximum length for the queue of pending
     connections.  If a connection request arrives with the queue full, the
     client may receive an error with an indication of ECONNREFUSED.  Alterna-
     tively, if the underlying protocol supports retransmission, the request
     may be ignored so that retries may succeed.

RREETTUURRNN VVAALLUUEESS
     The lliisstteenn() function returns the value 0 if successful; otherwise the
     value -1 is returned and the global variable _e_r_r_n_o is set to indicate the
     error.

EERRRROORRSS
     lliisstteenn() will fail if:

     [EACCES]           The current process has insufficient privileges.

     [EBADF]            The argument _s_o_c_k_e_t is not a valid file descriptor.

     [EDESTADDRREQ]     The socket is not bound to a local address and the
                        protocol does not support listening on an unbound
                        socket.

     [EINVAL]           _s_o_c_k_e_t is already connected.

     [ENOTSOCK]         The argument _s_o_c_k_e_t does not reference a socket.

     [EOPNOTSUPP]       The socket is not of a type that supports the opera-
                        tion lliisstteenn().

SSEEEE AALLSSOO
     accept(2), connect(2), connectx(2), socket(2)

BBUUGGSS
     The _b_a_c_k_l_o_g is currently limited (silently) to 128.

HHIISSTTOORRYY
     The lliisstteenn() function call appeared in 4.2BSD.

4.2 Berkeley Distribution       March 18, 2015       4.2 Berkeley Distribution
