//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:50
//  @file:      errno.h
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_COMPATIBLE_ERRNO_H
#define COMMON_BASE_COMPATIBLE_ERRNO_H


#include <errno.h>

#ifdef _WIN32

#if _MSC_VER < 1600 // before VC 2010
// new added since VC 2010
/* POSIX SUPPLEMENT */
#define EADDRINUSE      100
#define EADDRNOTAVAIL   101
#define EAFNOSUPPORT    102
#define EALREADY        103
#define EBADMSG         104
#define ECANCELED       105
#define ECONNABORTED    106
#define ECONNREFUSED    107
#define ECONNRESET      108
#define EDESTADDRREQ    109
#define EHOSTUNREACH    110
#define EIDRM           111
#define EINPROGRESS     112
#define EISCONN         113
#define ELOOP           114
#define EMSGSIZE        115
#define ENETDOWN        116
#define ENETRESET       117
#define ENETUNREACH     118
#define ENOBUFS         119
#define ENODATA         120
#define ENOLINK         121
#define ENOMSG          122
#define ENOPROTOOPT     123
#define ENOSR           124
#define ENOSTR          125
#define ENOTCONN        126
#define ENOTRECOVERABLE 127
#define ENOTSOCK        128
#define ENOTSUP         129
#define EOPNOTSUPP      130
#define EOTHER          131
#define EOVERFLOW       132
#define EOWNERDEAD      133
#define EPROTO          134
#define EPROTONOSUPPORT 135
#define EPROTOTYPE      136
#define ETIME           137
#define ETIMEDOUT       138
#define ETXTBSY         139
#define EWOULDBLOCK     140
#endif // _MSC_VER

// define to same value as WSAE* error codes
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define ESHUTDOWN               WSAESHUTDOWN
#define EHOSTDOWN               WSAEHOSTDOWN
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE

int WindowsErrorToPosixErrno(unsigned int error);

#endif // _WIN32

#endif // COMMON_BASE_COMPATIBLE_ERRNO_H
