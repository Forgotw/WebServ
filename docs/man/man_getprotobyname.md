GETPROTOENT(3)                                  Linux Programmer's Manual                                 GETPROTOENT(3)

NAME
       getprotoent, getprotobyname, getprotobynumber, setprotoent, endprotoent - get protocol entry

SYNOPSIS
       #include <netdb.h>

       struct protoent *getprotoent(void);

       struct protoent *getprotobyname(const char *name);

       struct protoent *getprotobynumber(int proto);

       void setprotoent(int stayopen);

       void endprotoent(void);

DESCRIPTION
       The getprotoent() function reads the next entry from the protocols database (see protocols(5)) and returns a pro‐
       toent structure containing the broken-out fields from the entry.  A connection is opened to the database if  nec‐
       essary.

       The  getprotobyname() function returns a protoent structure for the entry from the database that matches the pro‐
       tocol name name.  A connection is opened to the database if necessary.

       The getprotobynumber() function returns a protoent structure for the entry from the  database  that  matches  the
       protocol number number.  A connection is opened to the database if necessary.

       The  setprotoent()  function  opens a connection to the database, and sets the next entry to the first entry.  If
       stayopen is nonzero, then the connection to the database will not be closed between calls  to  one  of  the  get‐
       proto*() functions.

       The endprotoent() function closes the connection to the database.

       The protoent structure is defined in <netdb.h> as follows:

           struct protoent {
               char  *p_name;       /* official protocol name */
               char **p_aliases;    /* alias list */
               int    p_proto;      /* protocol number */
           }

       The members of the protoent structure are:

       p_name The official name of the protocol.

       p_aliases
              A NULL-terminated list of alternative names for the protocol.

       p_proto
              The protocol number.

RETURN VALUE
       The  getprotoent(), getprotobyname(), and getprotobynumber() functions return a pointer to a statically allocated
       protoent structure, or a null pointer if an error occurs or the end of the file is reached.

FILES
       /etc/protocols
              protocol database file

ATTRIBUTES
       For an explanation of the terms used in this section, see attributes(7).

       ┌───────────────────┬───────────────┬──────────────────────────────┐
       │Interface          │ Attribute     │ Value                        │
       ├───────────────────┼───────────────┼──────────────────────────────┤
       │getprotoent()      │ Thread safety │ MT-Unsafe race:protoent      │
       │                   │               │ race:protoentbuf locale      │
       ├───────────────────┼───────────────┼──────────────────────────────┤
       │getprotobyname()   │ Thread safety │ MT-Unsafe race:protobyname   │
       │                   │               │ locale                       │
       ├───────────────────┼───────────────┼──────────────────────────────┤
       │getprotobynumber() │ Thread safety │ MT-Unsafe race:protobynumber │
       │                   │               │ locale                       │
       ├───────────────────┼───────────────┼──────────────────────────────┤
       │setprotoent(),     │ Thread safety │ MT-Unsafe race:protoent      │
       │endprotoent()      │               │ locale                       │
       └───────────────────┴───────────────┴──────────────────────────────┘
       In the above table, protoent in race:protoent signifies that if  any  of  the  functions  setprotoent(),  getpro‐
       toent(), or endprotoent() are used in parallel in different threads of a program, then data races could occur.

CONFORMING TO
       POSIX.1-2001, POSIX.1-2008, 4.3BSD.

SEE ALSO
       getnetent(3), getprotoent_r(3), getservent(3), protocols(5)

COLOPHON
       This  page  is  part  of  release 5.10 of the Linux man-pages project.  A description of the project, information
       about reporting bugs, and the latest version of this page, can be found at https://www.kernel.org/doc/man-pages/.

GNU                                                    2020-12-21                                         GETPROTOENT(3)
