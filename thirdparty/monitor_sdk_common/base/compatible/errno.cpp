//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:51
//  @file:      errno.cpp
//  @brief:     
//
//********************************************************************


#ifdef _WIN32
#include <winsock2.h>
#include "thirdparty/monitor_sdk_common/base/compatible/errno.h"

// from MSVC crt/src/internal.h
extern "C"
void __cdecl _dosmaperr(unsigned long);

int WindowsErrorToPosixErrno(unsigned int error)
{
#define CASE_WSA(error) case WSA##error: mapped_errno = error; break
#define CASE_WSA_2(wsaerror, error) case WSA##wsaerror: mapped_errno = error; break
    int mapped_errno;
    switch (error)
    {
    CASE_WSA(EINTR);
    CASE_WSA(EBADF);
    CASE_WSA(EACCES);
    CASE_WSA(EFAULT);
    CASE_WSA(EINVAL);
    CASE_WSA(EMFILE);
    CASE_WSA(EWOULDBLOCK);
    CASE_WSA(EINPROGRESS);
    CASE_WSA(EALREADY);
    CASE_WSA(ENOTSOCK);
    CASE_WSA(EDESTADDRREQ);
    CASE_WSA(EMSGSIZE);
    CASE_WSA(EPROTOTYPE);
    CASE_WSA(ENOPROTOOPT);
    CASE_WSA(EPROTONOSUPPORT);
    CASE_WSA(ESOCKTNOSUPPORT);
    CASE_WSA(EOPNOTSUPP);
    CASE_WSA(EPFNOSUPPORT);
    CASE_WSA(EAFNOSUPPORT);
    CASE_WSA(EADDRINUSE);
    CASE_WSA(EADDRNOTAVAIL);
    CASE_WSA(ENETDOWN);
    CASE_WSA(ENETUNREACH);
    CASE_WSA(ENETRESET);
    CASE_WSA(ECONNABORTED);
    CASE_WSA(ECONNRESET);
    CASE_WSA(ENOBUFS);
    CASE_WSA(EISCONN);
    CASE_WSA(ENOTCONN);
    CASE_WSA(ESHUTDOWN);
//    CASE_WSA(ETOOMANYREFS);
    CASE_WSA(ETIMEDOUT);
    CASE_WSA(ECONNREFUSED);
    CASE_WSA(ELOOP);
    CASE_WSA(ENAMETOOLONG);
    CASE_WSA(EHOSTDOWN);
    CASE_WSA(EHOSTUNREACH);
    CASE_WSA(ENOTEMPTY);
    CASE_WSA(EPROCLIM);
    CASE_WSA(EUSERS);
    CASE_WSA(EDQUOT);
    CASE_WSA(ESTALE);
    CASE_WSA(EREMOTE);
    CASE_WSA_2(SYSNOTREADY, EAGAIN);
    CASE_WSA_2(VERNOTSUPPORTED, ENOSYS);
    CASE_WSA_2(NOTINITIALISED, EAGAIN);
    CASE_WSA_2(EDISCON, ESHUTDOWN);
    CASE_WSA_2(ENOMORE, ENODATA);
    CASE_WSA_2(ECANCELLED, ECANCELED);
//    CASE_WSA(SYSCALLFAILURE, EFAULT);
    CASE_WSA_2(SERVICE_NOT_FOUND, ENOSYS);
    CASE_WSA_2(TYPE_NOT_FOUND, ENOENT);
    CASE_WSA_2(_E_NO_MORE, ENODATA);
    CASE_WSA_2(_E_CANCELLED, ECANCELED);
    CASE_WSA_2(HOST_NOT_FOUND, ENOENT);
    CASE_WSA_2(TRY_AGAIN, EAGAIN);
    CASE_WSA_2(NO_DATA, ENODATA);
    default:
        _dosmaperr(error);
        return errno;
    }
    errno = mapped_errno;
    return mapped_errno;
}
#endif // _WIN32
