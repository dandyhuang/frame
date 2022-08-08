// Copyright 2010, Vivo Inc.

#include "thirdparty/monitor_sdk_common/base/compatible/io.h"

#if defined _MSC_VER
// Copied from http://www.mail-archive.com/bug-gnulib@gnu.org/msg11472.html

/* _get_osfhandle */
#include <io.h>

/* FlushFileBuffers */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <errno.h>

int fsync(int fd)
{
    HANDLE h = (HANDLE) _get_osfhandle(fd);
    DWORD err;

    if (h == INVALID_HANDLE_VALUE) {
        errno = EBADF;
        return -1;
    }

    if (!FlushFileBuffers(h)) {
        // Translate some Windows errors into rough approximations of Unix
        // errors.  MSDN is useless as usual - in this case it doesn't document
        // the full range of errors.
        err = GetLastError();
        switch (err) {
            /* eg. Trying to fsync a tty. */
        case ERROR_INVALID_HANDLE:
            errno = EINVAL;
            break;

        default:
            errno = EIO;
        }
        return -1;
    }

    return 0;
}
#endif // _MSC_VER
