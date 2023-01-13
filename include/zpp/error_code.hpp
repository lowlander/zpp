///
/// Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
///
/// SPDX-License-Identifier: Apache-2.0
///

#ifndef ZPP_INCLUDE_ZPP_ERROR_CODE_HPP
#define ZPP_INCLUDE_ZPP_ERROR_CODE_HPP

#include <kernel.h>
#include <sys/__assert.h>
#include <errno.h>

namespace zpp {

///
/// @brief enum with zephyr error codes
///
enum class error_code {
    k_perm            = EPERM,            ///< Not owner
    k_noent           = ENOENT,           ///< No such file or directory
    k_srch            = ESRCH,            ///< No such context
    k_intr            = EINTR,            ///< Interrupted system call
    k_io              = EIO,              ///< I/O error
    k_nxio            = ENXIO,            ///< No such device or address
    k_2big            = E2BIG,            ///< Arg list too long
    k_noexec          = ENOEXEC,          ///< Exec format error
    k_badf            = EBADF,            ///< Bad file number
    k_child           = ECHILD,           ///< No children
    k_again           = EAGAIN,           ///< No more contexts
    k_nomem           = ENOMEM,           ///< Not enough core
    k_acces           = EACCES,           ///< Permission denied
    k_fault           = EFAULT,           ///< Bad address
    k_notblk          = ENOTBLK,          ///< Block device required
    k_busy            = EBUSY,            ///< Mount device busy
    k_exits           = EEXIST,           ///< File exists
    k_xdev            = EXDEV,            ///< Cross-device link
    k_nodev           = ENODEV,           ///< No such device
    k_notdir          = ENOTDIR,          ///< Not a directory
    k_isdir           = EISDIR,           ///< Is a directory
    k_inval           = EINVAL,           ///< Invalid argument
    k_nfile           = ENFILE,           ///< File table overflow
    k_mfile           = EMFILE,           ///< Too many open files
    k_notty           = ENOTTY,           ///< Not a typewriter
    k_txtbsy          = ETXTBSY,          ///< Text file busy
    k_fbig            = EFBIG,            ///< File too large
    k_nospc           = ENOSPC,           ///< No space left on device
    k_spipe           = ESPIPE,           ///< Illegal seek
    k_rofs            = EROFS,            ///< Read-only file system
    k_mlink           = EMLINK,           ///< Too many links
    k_pipe            = EPIPE,            ///< Broken pipe
    k_dom             = EDOM,             ///< Argument too large
    k_range           = ERANGE,           ///< Result too large
    k_nomsg           = ENOMSG,           ///< Unexpected message type
    k_deadlk          = EDEADLK,          ///< Resource deadlock avoided
    k_nolck           = ENOLCK,           ///< No locks available
    k_nostr           = ENOSTR,           ///< STREAMS device required
    k_nodata          = ENODATA,          ///< Missing expected message data
    k_time            = ETIME,            ///< STREAMS timeout occurred
    k_nosr            = ENOSR,            ///< Insufficient memory
    k_proto           = EPROTO,           ///< Generic STREAMS error
    k_badmsg          = EBADMSG,          ///< Invalid STREAMS message
    k_nosys           = ENOSYS,           ///< Function not implemented
    k_notempty        = ENOTEMPTY,        ///< Directory not empty
    k_nametoolong     = ENAMETOOLONG,     ///< File name too long
    k_loop            = ELOOP,            ///< Too many levels of symbolic links
    k_opnotsupp       = EOPNOTSUPP,       ///< Operation not supported on socket
    k_pfnosupport     = EPFNOSUPPORT,     ///< Protocol family not supported
    k_connreset       = ECONNRESET,       ///< Connection reset by peer
    k_nobufs          = ENOBUFS,          ///< No buffer space available
    k_afnosupport     = EAFNOSUPPORT,     ///< Addr family not supported
    k_prototype       = EPROTOTYPE,       ///< Protocol wrong type for socket
    k_notsock         = ENOTSOCK,         ///< Socket operation on non-socket
    k_noprotoopt      = ENOPROTOOPT,      ///< Protocol not available
    k_shutdown        = ESHUTDOWN,        ///< Can’t send after socket shutdown
    k_connrefused     = ECONNREFUSED,     ///< Connection refused
    k_addrinuse       = EADDRINUSE,       ///< Address already in use
    k_connaborted     = ECONNABORTED,     ///< Software caused connection abort
    k_netunreach      = ENETUNREACH,      ///< Network is unreachable
    k_netdown         = ENETDOWN,         ///< Network is down
    k_timeout         = ETIMEDOUT,        ///< Connection timed out
    k_hostdown        = EHOSTDOWN,        ///< Host is down
    k_hostunreach     = EHOSTUNREACH,     ///< No route to host
    k_inprogress      = EINPROGRESS,      ///< Operation now in progress
    k_already         = EALREADY,         ///< Operation already in progress
    k_destaddrreq     = EDESTADDRREQ,     ///< Destination address required
    k_msgsize         = EMSGSIZE,         ///< Message size
    k_protonosupport  = EPROTONOSUPPORT,  ///< Protocol not supported
    k_socktnosupport  = ESOCKTNOSUPPORT,  ///< Socket type not supported
    k_addrnotavail    = EADDRNOTAVAIL,    ///< Can’t assign requested address
    k_netreset        = ENETRESET,        ///< Network dropped connection on reset
    k_isconn          = EISCONN,          ///< Socket is already connected
    k_notconn         = ENOTCONN,         ///< Socket is not connected
    k_toomanyrefs     = ETOOMANYREFS,     ///< Too many references: can’t splice
    k_notsup          = ENOTSUP,          ///< Unsupported value
    k_ilseq           = EILSEQ,           ///< Illegal byte sequence
    k_overflow        = EOVERFLOW,        ///< Value overflow
    k_canceled        = ECANCELED,        ///< Operation canceled
    k_wouldblock      = EWOULDBLOCK,      ///< Operation would block
};

constexpr error_code to_error_code(int v) noexcept
{
  return static_cast<error_code>(v);
}

static_assert(error_code::k_perm == to_error_code(EPERM));
static_assert(error_code::k_wouldblock == to_error_code(EWOULDBLOCK));

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_ERROR_CODE_HPP
