
SELECT(2)                   BSD System Calls Manual                  SELECT(2)

NNAAMMEE
     FFDD__CCLLRR, FFDD__CCOOPPYY, FFDD__IISSSSEETT, FFDD__SSEETT, FFDD__ZZEERROO, sseelleecctt -- synchronous I/O
     multiplexing

SSYYNNOOPPSSIISS
     ##iinncclluuddee <<ssyyss//sseelleecctt..hh>>

     _v_o_i_d
     FFDD__CCLLRR(_f_d, _f_d___s_e_t _*_f_d_s_e_t);

     _v_o_i_d
     FFDD__CCOOPPYY(_f_d___s_e_t _*_f_d_s_e_t___o_r_i_g, _f_d___s_e_t _*_f_d_s_e_t___c_o_p_y);

     _i_n_t
     FFDD__IISSSSEETT(_f_d, _f_d___s_e_t _*_f_d_s_e_t);

     _v_o_i_d
     FFDD__SSEETT(_f_d, _f_d___s_e_t _*_f_d_s_e_t);

     _v_o_i_d
     FFDD__ZZEERROO(_f_d___s_e_t _*_f_d_s_e_t);

     _i_n_t
     sseelleecctt(_i_n_t _n_f_d_s, _f_d___s_e_t _*_r_e_s_t_r_i_c_t _r_e_a_d_f_d_s, _f_d___s_e_t _*_r_e_s_t_r_i_c_t _w_r_i_t_e_f_d_s,
         _f_d___s_e_t _*_r_e_s_t_r_i_c_t _e_r_r_o_r_f_d_s, _s_t_r_u_c_t _t_i_m_e_v_a_l _*_r_e_s_t_r_i_c_t _t_i_m_e_o_u_t);

DDEESSCCRRIIPPTTIIOONN
     sseelleecctt() examines the I/O descriptor sets whose addresses are passed in
     _r_e_a_d_f_d_s, _w_r_i_t_e_f_d_s, and _e_r_r_o_r_f_d_s to see if some of their descriptors are
     ready for reading, are ready for writing, or have an exceptional condi-
     tion pending, respectively.  The first _n_f_d_s descriptors are checked in
     each set; i.e., the descriptors from 0 through _n_f_d_s-1 in the descriptor
     sets are examined.  (Example: If you have set two file descriptors "4"
     and "17", _n_f_d_s should  not be "2", but rather "17 + 1" or "18".)  On
     return, sseelleecctt() replaces the given descriptor sets with subsets consist-
     ing of those descriptors that are ready for the requested operation.
     sseelleecctt() returns the total number of ready descriptors in all the sets.

     The descriptor sets are stored as bit fields in arrays of integers.  The
     following macros are provided for manipulating such descriptor sets:
     FFDD__ZZEERROO(_&_f_d_s_e_t) initializes a descriptor set _f_d_s_e_t to the null set.
     FFDD__SSEETT(_f_d, _&_f_d_s_e_t) includes a particular descriptor _f_d in _f_d_s_e_t.
     FFDD__CCLLRR(_f_d, _&_f_d_s_e_t) removes _f_d from _f_d_s_e_t.  FFDD__IISSSSEETT(_f_d, _&_f_d_s_e_t) is non-
     zero if _f_d is a member of _f_d_s_e_t, zero otherwise.  FFDD__CCOOPPYY(_&_f_d_s_e_t___o_r_i_g,
     _&_f_d_s_e_t___c_o_p_y) replaces an already allocated _&_f_d_s_e_t___c_o_p_y file descriptor
     set with a copy of _&_f_d_s_e_t___o_r_i_g.  The behavior of these macros is unde-
     fined if a descriptor value is less than zero or greater than or equal to
     FD_SETSIZE, which is normally at least equal to the maximum number of
     descriptors supported by the system.

     If _t_i_m_e_o_u_t is a non-nil pointer, it specifies a maximum interval to wait
     for the selection to complete.  If _t_i_m_e_o_u_t is a nil pointer, the select
     blocks indefinitely.  To effect a poll, the _t_i_m_e_o_u_t argument should be
     non-nil, pointing to a zero-valued timeval structure.  _T_i_m_e_o_u_t is not
     changed by sseelleecctt(), and may be reused on subsequent calls, however it is
     good style to re-initialize it before each invocation of sseelleecctt().

     Any of _r_e_a_d_f_d_s, _w_r_i_t_e_f_d_s, and _e_r_r_o_r_f_d_s may be given as nil pointers if no
     descriptors are of interest.

RREETTUURRNN VVAALLUUEESS
     sseelleecctt() returns the number of ready descriptors that are contained in
     the descriptor sets, or -1 if an error occurred.  If the time limit
     expires, sseelleecctt() returns 0.  If sseelleecctt() returns with an error, includ-
     ing one due to an interrupted call, the descriptor sets will be unmodi-
     fied and the global variable _e_r_r_n_o will be set to indicate the error.

EERRRROORRSS
     An error return from sseelleecctt() indicates:

     [EAGAIN]           The kernel was (perhaps temporarily) unable to allo-
                        cate the requested number of file descriptors.

     [EBADF]            One of the descriptor sets specified an invalid
                        descriptor.

     [EINTR]            A signal was delivered before the time limit expired
                        and before any of the selected events occurred.

     [EINVAL]           The specified time limit is invalid.  One of its com-
                        ponents is negative or too large.

     [EINVAL]           _n_d_f_s is greater than FD_SETSIZE and _DARWIN_UNLIM-
                        ITED_SELECT is not defined.

LLEEGGAACCYY SSYYNNOOPPSSIISS
     ##iinncclluuddee <<ssyyss//sseelleecctt..hh>>
           - or -
     ##iinncclluuddee <<ssyyss//ttyyppeess..hh>>
     ##iinncclluuddee <<ssyyss//ttiimmee..hh>>
     ##iinncclluuddee <<uunniissttdd..hh>>

     FFDD__SSEETT(_f_d, _&_f_d_s_e_t);

     FFDD__CCLLRR(_f_d, _&_f_d_s_e_t);

     FFDD__IISSSSEETT(_f_d, _&_f_d_s_e_t);

     FFDD__CCOOPPYY(_&_f_d_s_e_t___o_r_i_g, _&_f_d_s_e_t___c_o_p_y);

     FFDD__ZZEERROO(_&_f_d_s_e_t);

CCOOMMPPAATTIIBBIILLIITTYY
     sseelleecctt() now returns with _e_r_r_n_o set to EINVAL when _n_f_d_s is greater than
     FD_SETSIZE.  Use a smaller value for _n_f_d_s or compile with -D_DAR-
     WIN_UNLIMITED_SELECT.

SSEEEE AALLSSOO
     accept(2), connect(2), connectx(2), getdtablesize(2), gettimeofday(2),
     read(2), recv(2), send(2), write(2), compat(5)

BBUUGGSS
     Although the provision of getdtablesize(2) was intended to allow user
     programs to be written independent of the kernel limit on the number of
     open files, the dimension of a sufficiently large bit field for select
     remains a problem.  The default size FD_SETSIZE (currently 1024) is some-
     what smaller than the current kernel limit to the number of open files.
     However, in order to accommodate programs which might potentially use a
     larger number of open files with select, it is possible to increase this
     size within a program by providing a larger definition of FD_SETSIZE
     before the inclusion of <_s_y_s_/_t_y_p_e_s_._h>.

     sseelleecctt() should probably have been designed to return the time remaining
     from the original timeout, if any, by modifying the time value in place.
     However, it is unlikely this semantic will ever be implemented, as the
     change would cause source code compatibility problems.  In general it is
     unwise to assume that the timeout value will be unmodified by the
     sseelleecctt() call, and the caller should reinitialize it on each invocation.

HHIISSTTOORRYY
     The sseelleecctt() function call appeared in 4.2BSD.

4.2 Berkeley Distribution       March 18, 2015       4.2 Berkeley Distribution
