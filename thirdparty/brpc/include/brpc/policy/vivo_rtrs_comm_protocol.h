// Authors: weitao.zhang@dandy.com

#ifndef BRPC_POLICY_VIVO_RTRS_COMM_PROTOCAL_
#define BRPC_POLICY_VIVO_RTRS_COMM_PROTOCAL_

#include "brpc/protocol.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/io/coded_stream.h>
#include "butil/iobuf.h"                         // butil::IOBuf
#include "butil/sys_byteorder.h"

namespace brpc {
namespace policy {

#define ADPOS_INFO_FIX_LEN 36
#define  STD_DWORD_NULL         0xFFFFFFFF
#define  STD_DWORD_ALL          0xFFFFFFFE

extern const uint32_t header_size;
extern const uint32_t expected_ad_pos_info_size;
struct AlgID
{
	AlgID()
	{
		version = 0;
		subversion = 0;
		fileversion = 0;
	}

	uint32_t version;
	uint32_t subversion;
	uint32_t fileversion;

	std::string toString() const
	{
		std::ostringstream oss;
		oss<<version<<"("<<subversion<<")";

		return oss.str();
	}

	std::string toFullString() const
	{
		std::ostringstream oss;
		oss << version << "." << subversion << "." <<fileversion;

		return oss.str();
	}

	bool EncodeTo(google::protobuf::io::CodedOutputStream& outstream);

	bool DecodeFrom(google::protobuf::io::CodedInputStream& instream);

	bool operator<(const AlgID& rhs) const
	{
		if (version < rhs.version)
		{
			return true;
		}
		else if (version > rhs.version)
		{
			return false;
		}
		else if (subversion != rhs.subversion && subversion != STD_DWORD_NULL && rhs.subversion != STD_DWORD_NULL)
		{
			return subversion < rhs.subversion;
		}
		else if (fileversion != rhs.fileversion && fileversion != STD_DWORD_NULL && rhs.fileversion != STD_DWORD_NULL)
		{
			return fileversion < rhs.fileversion;
		}

		return false;
	}

	bool operator==(const AlgID& rhs) const
	{
		return version == rhs.version && subversion == rhs.subversion && fileversion == rhs.fileversion;
	}

	AlgID& operator=(const AlgID& rhs)
	{
		version = rhs.version;
		subversion = rhs.subversion;
		fileversion = rhs.fileversion;
		return *this;
	}

    bool operator !=(const AlgID& rhs) const
    {
        	return version != rhs.version || subversion != rhs.subversion || fileversion != rhs.fileversion;
   	 }
};

struct AccessFlowTag
{
	uint32_t totallen;
	uint32_t bid;//业务bid
	std::string uid;
	unsigned char uidtype;
    uint32_t site_id; //站点集合
	std::string trace_id;
	char unknow[0];

	bool DecodeFromBuffer(google::protobuf::io::CodedInputStream& instream);

	bool EncodeToBuffer(google::protobuf::io::CodedOutputStream& outstream);

	AccessFlowTag() : totallen(0),bid(0),uid(""),uidtype(0),site_id(0),trace_id("")
    {
    };

	AccessFlowTag& operator=(const AccessFlowTag& rhs)
	{
		totallen = rhs.totallen;
		bid = rhs.bid;
		uid = rhs.uid;
		uidtype = rhs.uidtype;
		trace_id = rhs.trace_id;
		return *this;
	}
};

struct AdposInfo
{
	uint32_t totallen;
	uint64_t adposid;
	uint32_t siteid;
	AlgID algid;
	uint32_t adidnum;
	uint32_t relfect_adpos;
	char unknow[0];	//unknow

	std::string access_only; //for access so,not transmit to compute.

	bool EncodeTo(google::protobuf::io::CodedOutputStream& outstream);

	bool DecodeFrom(google::protobuf::io::CodedInputStream& instream);

	bool operator< (const AdposInfo& info) const
	{
		if  (adposid < info.adposid)
		{
			return true;
		}

		return false;
	}

	AdposInfo& operator=(const AdposInfo& rhs)
	{
		totallen = rhs.totallen;
		adposid = rhs.adposid;
		algid = rhs.algid;
		adidnum = rhs.adidnum;
		relfect_adpos = rhs.relfect_adpos;
		return *this;
	}

	AdposInfo() : adposid(0),siteid(0),adidnum(0)
	,relfect_adpos(1)
	{
		//fixlen
		//totallen = sizeof(uint32_t)*7 + sizeof(uint64_t);
		totallen = ADPOS_INFO_FIX_LEN;
	}
};

class VivoRawPacker {
public:
    // Notice: User must guarantee `stream' is as long as the packed data.
    explicit VivoRawPacker(void* stream) : _stream((char*)stream) {}
    ~VivoRawPacker() {}

    // Not using operator<< because some values may be packed differently from
    // its type.
    VivoRawPacker& pack32(uint32_t host_value) {
        *(uint32_t*)_stream = butil::HostToNet32(host_value);
        _stream += 4;
        return *this;
    }

    VivoRawPacker& pack64(uint64_t host_value) {
        uint32_t *p = (uint32_t*)_stream;
        p[0] = butil::HostToNet32(host_value >> 32);
        p[1] = butil::HostToNet32(host_value & 0xFFFFFFFF);
        _stream += 8;
        return *this;
    }

    VivoRawPacker& pack8(char host_value) {
        *_stream = host_value;
        _stream += 1;
        return *this;
    }

private:
    char* _stream;
};

class VivoRawUnpacker {

public:
    explicit VivoRawUnpacker(const void* stream) : _stream((const char*)stream) {}
    ~VivoRawUnpacker() {}

    VivoRawUnpacker& unpack32(uint32_t & host_value) {
        host_value = butil::NetToHost32(*(const uint32_t*)_stream);
        _stream += 4;
        return *this;
    }

    VivoRawUnpacker& unpack64(uint64_t & host_value) {
        const uint32_t *p = (const uint32_t*)_stream;
        host_value = (((uint64_t)butil::NetToHost32(p[0])) << 32) | butil::NetToHost32(p[1]);
        _stream += 8;
        return *this;
    }

    VivoRawUnpacker& unpack8(char & host_value) {
        host_value = *_stream;
        _stream += 1;
        return *this;
    }    

private:
    const char* _stream;
};

// Parse binary format of rtrs common
ParseResult ParseVivoRtrsCommMessage(butil::IOBuf* source, Socket *socket, bool read_eof,
                            const void *arg);

// Actions to a (client) request in rtrs common format
void ProcessVivoRtrsCommRequest(InputMessageBase* msg);

// Actions to a (server) response in rtrs common format.
void ProcessVivoRtrsCommResponse(InputMessageBase* msg);

// Verify authentication information in rtrs common format
bool VerifyVivoRtrsCommRequest(const InputMessageBase* msg);

// Pack `request' to `method' into `buf'.
void PackVivoRtrsCommRequest(butil::IOBuf* buf,
                    SocketMessage**,
                    uint64_t correlation_id,
                    const google::protobuf::MethodDescriptor* method,
                    Controller* controller,
                    const butil::IOBuf& request,
                    const Authenticator* auth);

}  // namespace policy
} // namespace brpc

#endif  // BRPC_POLICY_VIVO_RTRS_COMM_PROTOCAL_
