
NC(1)                     BSD General Commands Manual                    NC(1)

NNAAMMEE
     nncc -- arbitrary TCP and UDP connections and listens

SSYYNNOOPPSSIISS
     nncc [--4466AAccDDCCddhhkkllnnrrttUUuuvvzz] [--bb _b_o_u_n_d_i_f] [--ii _i_n_t_e_r_v_a_l] [--pp _s_o_u_r_c_e___p_o_r_t]
        [--ss _s_o_u_r_c_e___i_p___a_d_d_r_e_s_s] [--ww _t_i_m_e_o_u_t] [--XX _p_r_o_x_y___p_r_o_t_o_c_o_l] [--xx
        _p_r_o_x_y___a_d_d_r_e_s_s[:_p_o_r_t]] [----aappppllee--ddeelleeggaattee--ppiidd _p_i_d]
        [----aappppllee--ddeelleeggaattee--uuuuiidd _u_u_i_d] [----aappppllee--eexxtt--bbkk--iiddllee]
        [----aappppllee--nnoowwaakkeeffrroommsslleeeepp] [----aappppllee--eeccnn _m_o_d_e] [_h_o_s_t_n_a_m_e] [_p_o_r_t[_s]]

DDEESSCCRRIIPPTTIIOONN
     The nncc (or nneettccaatt) utility is used for just about anything under the sun
     involving TCP or UDP.  It can open TCP connections, send UDP packets,
     listen on arbitrary TCP and UDP ports, do port scanning, and deal with
     both IPv4 and IPv6.  Unlike telnet(1), nncc scripts nicely, and separates
     error messages onto standard error instead of sending them to standard
     output, as telnet(1) does with some.

     Common uses include:

           ++oo   simple TCP proxies
           ++oo   shell-script based HTTP clients and servers
           ++oo   network daemon testing
           ++oo   a SOCKS or HTTP ProxyCommand for ssh(1)
           ++oo   and much, much more

     The options are as follows:

     --44      Forces nncc to use IPv4 addresses only.

     --66      Forces nncc to use IPv6 addresses only.

     --AA      Set SO_RECV_ANYIF on socket.

     --bb _b_o_u_n_d_i_f
             Specifies the interface to bind the socket to.

     --cc      Send CRLF as line-ending

     --DD      Enable debugging on the socket.

     --CC      Forces nncc not to use cellular data context.

     --dd      Do not attempt to read from stdin.

     --hh      Prints out nncc help.

     --ii _i_n_t_e_r_v_a_l
             Specifies a delay time interval between lines of text sent and
             received.  Also causes a delay time between connections to multi-
             ple ports.

     --GG _c_o_n_n_t_i_m_e_o_u_t
             TCP connection timeout in seconds.

     --HH _k_e_e_p_i_d_l_e
             Initial TCP keep alive timeout in seconds.

     --II _k_e_e_p_i_n_t_v_l
             Interval for repeating TCP keep alive timeouts in seconds.

     --JJ _k_e_e_p_c_n_t
             Number of times to repeat TCP keep alive packets.

     --kk      Forces nncc to stay listening for another connection after its cur-
             rent connection is completed.  It is an error to use this option
             without the --ll option.

     --ll      Used to specify that nncc should listen for an incoming connection
             rather than initiate a connection to a remote host.  It is an
             error to use this option in conjunction with the --pp, --ss, or --zz
             options.  Additionally, any timeouts specified with the --ww option
             are ignored.

     --LL _n_u_m___p_r_o_b_e_s
             Number of probes to send to the peer before declaring that the
             peer is not reachable and generating an adpative timeout
             read/write event.

     --nn      Do not do any DNS or service lookups on any specified addresses,
             hostnames or ports.

     --pp _s_o_u_r_c_e___p_o_r_t
             Specifies the source port nncc should use, subject to privilege
             restrictions and availability.  It is an error to use this option
             in conjunction with the --ll option.

     --rr      Specifies that source and/or destination ports should be chosen
             randomly instead of sequentially within a range or in the order
             that the system assigns them.

     --ss _s_o_u_r_c_e___i_p___a_d_d_r_e_s_s
             Specifies the IP of the interface which is used to send the pack-
             ets.  It is an error to use this option in conjunction with the
             --ll option.

     --tt      Causes nncc to send RFC 854 DON'T and WON'T responses to RFC 854 DO
             and WILL requests.  This makes it possible to use nncc to script
             telnet sessions.

     --UU      Specifies to use Unix Domain Sockets.

     --uu      Use UDP instead of the default option of TCP.

     --vv      Have nncc give more verbose output.

     --ww _t_i_m_e_o_u_t
             If a connection and stdin are idle for more than _t_i_m_e_o_u_t seconds,
             then the connection is silently closed.  The --ww flag has no
             effect on the --ll option, i.e. nncc will listen forever for a con-
             nection, with or without the --ww flag.  The default is no timeout.

     --XX _p_r_o_x_y___v_e_r_s_i_o_n
             Requests that nncc should use the specified protocol when talking
             to the proxy server.  Supported protocols are ``4'' (SOCKS v.4),
             ``5'' (SOCKS v.5) and ``connect'' (HTTPS proxy).  If the protocol
             is not specified, SOCKS version 5 is used.

     --xx _p_r_o_x_y___a_d_d_r_e_s_s[:_p_o_r_t]
             Requests that nncc should connect to _h_o_s_t_n_a_m_e using a proxy at
             _p_r_o_x_y___a_d_d_r_e_s_s and _p_o_r_t.  If _p_o_r_t is not specified, the well-known
             port for the proxy protocol is used (1080 for SOCKS, 3128 for
             HTTPS).

     --zz      Specifies that nncc should just scan for listening daemons, without
             sending any data to them.  It is an error to use this option in
             conjunction with the --ll option.

     ----aappppllee--ddeelleeggaattee--ppiidd
             Requests that nncc should delegate the socket for the specified
             PID.  It is an error to use this option in conjunction with the
             ----aappppllee--ddeelleeggaattee--uuuuiidd option.

     ----aappppllee--ddeelleeggaattee--uuuuiidd
             Requests that nncc should delegate the socket for the specified
             UUID.  It is an error to use this option in conjunction with the
             ----aappppllee--ddeelleeggaattee--ppiidd option.

     ----aappppllee--eexxtt--bbkk--iiddllee
             Requests that nncc marks its socket for extended background idle
             time when the process becomes suspended.

     ----aappppllee--nnoowwaakkeeffrroommsslleeeepp
             Requests that nncc marks its socket to exlude the local port from
             the list of opened ports that is queried by drivers when the sys-
             tem goes to sleep.

     ----eeccnn   Requests that nncc marks to use the socket option TCP_ECN_MODE to
             set the ECN mode (default, enable, disable)

     _h_o_s_t_n_a_m_e can be a numerical IP address or a symbolic hostname (unless the
     --nn option is given).  In general, a hostname must be specified, unless
     the --ll option is given (in which case the local host is used).

     _p_o_r_t[_s] can be single integers or ranges.  Ranges are in the form nn-mm.
     In general, a destination port must be specified, unless the --UU option is
     given (in which case a socket must be specified).

CCLLIIEENNTT//SSEERRVVEERR MMOODDEELL
     It is quite simple to build a very basic client/server model using nncc.
     On one console, start nncc listening on a specific port for a connection.
     For example:

           $ nc -l 1234

     nncc is now listening on port 1234 for a connection.  On a second console
     (or a second machine), connect to the machine and port being listened on:

           $ nc 127.0.0.1 1234

     There should now be a connection between the ports.  Anything typed at
     the second console will be concatenated to the first, and vice-versa.
     After the connection has been set up, nncc does not really care which side
     is being used as a `server' and which side is being used as a `client'.
     The connection may be terminated using an EOF (`^D').

DDAATTAA TTRRAANNSSFFEERR
     The example in the previous section can be expanded to build a basic data
     transfer model.  Any information input into one end of the connection
     will be output to the other end, and input and output can be easily cap-
     tured in order to emulate file transfer.

     Start by using nncc to listen on a specific port, with output captured into
     a file:

           $ nc -l 1234 > filename.out

     Using a second machine, connect to the listening nncc process, feeding it
     the file which is to be transferred:

           $ nc host.example.com 1234 < filename.in

     After the file has been transferred, the connection will close automati-
     cally.

TTAALLKKIINNGG TTOO SSEERRVVEERRSS
     It is sometimes useful to talk to servers ``by hand'' rather than through
     a user interface.  It can aid in troubleshooting, when it might be neces-
     sary to verify what data a server is sending in response to commands
     issued by the client.  For example, to retrieve the home page of a web
     site:

           $ echo -n "GET / HTTP/1.0\r\n\r\n" | nc host.example.com 80

     Note that this also displays the headers sent by the web server.  They
     can be filtered, using a tool such as sed(1), if necessary.

     More complicated examples can be built up when the user knows the format
     of requests required by the server.  As another example, an email may be
     submitted to an SMTP server using:

           $ nc localhost 25 << EOF
           HELO host.example.com
           MAIL FROM: <user@host.example.com>
           RCPT TO: <user2@host.example.com>
           DATA
           Body of email.
           .
           QUIT
           EOF

PPOORRTT SSCCAANNNNIINNGG
     It may be useful to know which ports are open and running services on a
     target machine.  The --zz flag can be used to tell nncc to report open ports,
     rather than initiate a connection.  For example:

           $ nc -z host.example.com 20-30
           Connection to host.example.com 22 port [tcp/ssh] succeeded!
           Connection to host.example.com 25 port [tcp/smtp] succeeded!

     The port range was specified to limit the search to ports 20 - 30.

     Alternatively, it might be useful to know which server software is run-
     ning, and which versions.  This information is often contained within the
     greeting banners.  In order to retrieve these, it is necessary to first
     make a connection, and then break the connection when the banner has been
     retrieved.  This can be accomplished by specifying a small timeout with
     the --ww flag, or perhaps by issuing a "QUIT" command to the server:

           $ echo "QUIT" | nc host.example.com 20-30
           SSH-1.99-OpenSSH_3.6.1p2
           Protocol mismatch.
           220 host.example.com IMS SMTP Receiver Version 0.84 Ready

EEXXAAMMPPLLEESS
     Open a TCP connection to port 42 of host.example.com, using port 31337 as
     the source port, with a timeout of 5 seconds:

           $ nc -p 31337 -w 5 host.example.com 42

     Open a UDP connection to port 53 of host.example.com:

           $ nc -u host.example.com 53

     Open a TCP connection to port 42 of host.example.com using 10.1.2.3 as
     the IP for the local end of the connection:

           $ nc -s 10.1.2.3 host.example.com 42

     Create and listen on a Unix Domain Socket:

           $ nc -lU /var/tmp/dsocket

     Connect to port 42 of host.example.com via an HTTP proxy at 10.2.3.4,
     port 8080.  This example could also be used by ssh(1); see the
     PPrrooxxyyCCoommmmaanndd directive in ssh_config(5) for more information.

           $ nc -x10.2.3.4:8080 -Xconnect host.example.com 42

SSEEEE AALLSSOO
     cat(1), ssh(1)

AAUUTTHHOORRSS
     Original implementation by *Hobbit* <hobbit@avian.org>.
     Rewritten with IPv6 support by Eric Jackson <ericj@monkey.org>.

CCAAVVEEAATTSS
     UDP port scans will always succeed (i.e. report the port as open), ren-
     dering the --uuzz combination of flags relatively useless.

BSD                              June 25, 2001                             BSD
