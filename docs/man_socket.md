
SOCKET(2)                   BSD System Calls Manual                  SOCKET(2)

NNAAMMEE
     ssoocckkeett -- create an endpoint for communication

SSYYNNOOPPSSIISS
     ##iinncclluuddee <<ssyyss//ssoocckkeett..hh>>

     _i_n_t
     ssoocckkeett(_i_n_t _d_o_m_a_i_n, _i_n_t _t_y_p_e, _i_n_t _p_r_o_t_o_c_o_l);

DDEESSCCRRIIPPTTIIOONN
     ssoocckkeett() creates an endpoint for communication and returns a descriptor.

     The _d_o_m_a_i_n parameter specifies a communications domain within which com-
     munication will take place; this selects the protocol family which should
     be used.  These families are defined in the include file <_s_y_s_/_s_o_c_k_e_t_._h>.
     The currently understood formats are

           PF_LOCAL        Host-internal protocols, formerly called PF_UNIX,
           PF_UNIX         Host-internal protocols, deprecated, use PF_LOCAL,
           PF_INET         Internet version 4 protocols,
           PF_ROUTE        Internal Routing protocol,
           PF_KEY          Internal key-management function,
           PF_INET6        Internet version 6 protocols,
           PF_SYSTEM       System domain,
           PF_NDRV         Raw access to network device

     The socket has the indicated _t_y_p_e, which specifies the semantics of com-
     munication.  Currently defined types are:

           SOCK_STREAM
           SOCK_DGRAM
           SOCK_RAW

     A SOCK_STREAM type provides sequenced, reliable, two-way connection based
     byte streams.  An out-of-band data transmission mechanism may be sup-
     ported.  A SOCK_DGRAM socket supports datagrams (connectionless, unreli-
     able messages of a fixed (typically small) maximum length).  SOCK_RAW
     sockets provide access to internal network protocols and interfaces.  The
     type SOCK_RAW, which is available only to the super-user.

     The _p_r_o_t_o_c_o_l specifies a particular protocol to be used with the socket.
     Normally only a single protocol exists to support a particular socket
     type within a given protocol family.  However, it is possible that many
     protocols may exist, in which case a particular protocol must be speci-
     fied in this manner.  The protocol number to use is particular to the
     communication domain in which communication is to take place; see
     protocols(5).

     Sockets of type SOCK_STREAM are full-duplex byte streams, similar to
     pipes.  A stream socket must be in a _c_o_n_n_e_c_t_e_d state before any data may
     be sent or received on it.  A connection to another socket is created
     with a connect(2) or connectx(2) call.  Once connected, data may be
     transferred using read(2) and write(2) calls or some variant of the
     send(2) and recv(2) calls.  When a session has been completed a close(2)
     may be performed.  Out-of-band data may also be transmitted as described
     in send(2) and received as described in recv(2).

     The communications protocols used to implement a SOCK_STREAM insure that
     data is not lost or duplicated.  If a piece of data for which the peer
     protocol has buffer space cannot be successfully transmitted within a
     reasonable length of time, then the connection is considered broken and
     calls will indicate an error with -1 returns and with ETIMEDOUT as the
     specific code in the global variable _e_r_r_n_o.  The protocols optionally
     keep sockets ``warm'' by forcing transmissions roughly every minute in
     the absence of other activity.  An error is then indicated if no response
     can be elicited on an otherwise idle connection for a extended period
     (e.g. 5 minutes).  A SIGPIPE signal is raised if a process sends on a
     broken stream; this causes naive processes, which do not handle the sig-
     nal, to exit.

     SOCK_DGRAM and SOCK_RAW sockets allow sending of datagrams to correspon-
     dents named in send(2) calls.  Datagrams are generally received with
     recvfrom(2), which returns the next datagram with its return address.

     An fcntl(2) call can be used to specify a process group to receive a
     SIGURG signal when the out-of-band data arrives.  It may also enable non-
     blocking I/O and asynchronous notification of I/O events via SIGIO.

     The operation of sockets is controlled by socket level _o_p_t_i_o_n_s.  These
     options are defined in the file <_s_y_s_/_s_o_c_k_e_t_._h>.  Setsockopt(2) and
     getsockopt(2) are used to set and get options, respectively.

RREETTUURRNN VVAALLUUEESS
     A -1 is returned if an error occurs, otherwise the return value is a
     descriptor referencing the socket.

EERRRROORRSS
     The ssoocckkeett() system call fails if:

     [EACCES]           Permission to create a socket of the specified type
                        and/or protocol is denied.

     [EAFNOSUPPORT]     The specified address family is not supported.

     [EMFILE]           The per-process descriptor table is full.

     [ENFILE]           The system file table is full.

     [ENOBUFS]          Insufficient buffer space is available.  The socket
                        cannot be created until sufficient resources are
                        freed.

     [ENOMEM]           Insufficient memory was available to fulfill the
                        request.

     [EPROTONOSUPPORT]  The protocol type or the specified protocol is not
                        supported within this domain.

     [EPROTOTYPE]       The socket type is not supported by the protocol.

     If a new protocol family is defined, the socreate process is free to
     return any desired error code.  The ssoocckkeett() system call will pass this
     error code along (even if it is undefined).

LLEEGGAACCYY SSYYNNOOPPSSIISS
     ##iinncclluuddee <<ssyyss//ttyyppeess..hh>>
     ##iinncclluuddee <<ssyyss//ssoocckkeett..hh>>

     The include file <_s_y_s_/_t_y_p_e_s_._h> is necessary.

SSEEEE AALLSSOO
     accept(2), bind(2), connect(2), connectx(2), disconnectx(2),
     getsockname(2), getsockopt(2), ioctl(2), listen(2), read(2), recv(2),
     select(2), send(2), shutdown(2), socketpair(2), write(2), getprotoent(3),
     inet(4), inet6(4), unix(4), compat(5)

     _A_n _I_n_t_r_o_d_u_c_t_o_r_y _4_._3 _B_S_D _I_n_t_e_r_p_r_o_c_e_s_s _C_o_m_m_u_n_i_c_a_t_i_o_n _T_u_t_o_r_i_a_l, reprinted in
     UNIX Programmer's Supplementary Documents Volume 1.

     _B_S_D _I_n_t_e_r_p_r_o_c_e_s_s _C_o_m_m_u_n_i_c_a_t_i_o_n _T_u_t_o_r_i_a_l, reprinted in UNIX Programmer's
     Supplementary Documents Volume 1.

HHIISSTTOORRYY
     The ssoocckkeett() function call appeared in 4.2BSD.

BSD                             March 18, 2015                             BSD
