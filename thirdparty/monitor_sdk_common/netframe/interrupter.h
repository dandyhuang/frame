// Copyright (c) 2010.
// All rights reserved.
//
//

#ifndef COMMON_NETFRAME_INTERRUPTER_H
#define COMMON_NETFRAME_INTERRUPTER_H

#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>

namespace common {
namespace netframe {

class Interrupter
{
protected:
    Interrupter() {}
public:
    /// Destructor.
    virtual ~Interrupter() { }

    virtual bool Create() = 0;

    /// Interrupt the select call.
    virtual bool Interrupt() = 0;

    /// Reset the select interrupt. Returns true if the call was interrupted.
    virtual bool Reset() = 0;

    /// Get the read descriptor to be passed to select.
    virtual int GetReadFd() const = 0;
};

} // namespace netframe
} // namespace common

#endif // COMMON_NETFRAME_INTERRUPTER_H

