//
// ICMPEventArgs.h
//
// $Id: //poco/1.3/Net/include/Poco/Net/ICMPEventArgs.h#1 $
//
// Library: Net
// Package: ICMP
// Module:  ICMPEventArgs
//
// Definition of ICMPEventArgs.
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef Net_ICMPEventArgs_INCLUDED
#define Net_ICMPEventArgs_INCLUDED


#include "Poco/Net/Net.h"
#include "Poco/Net/SocketAddress.h"
#include <vector>
#undef min // macros collide with valarray::min() and valarray::max()
#undef max
#include <valarray>


namespace Poco {
namespace Net {


class Net_API ICMPEventArgs
	/// The purpose of the ICMPEventArgs class is to be used as template parameter
	/// to instantiate event members in ICMPClient class.
	/// When clients register for an event notification, the reference to the class is 
	///	passed to the handler function to provide information about the event.
{
public:
	ICMPEventArgs(const SocketAddress& address, int repetitions, int dataSize, int ttl);
		/// Creates ICMPEventArgs.

	virtual ~ICMPEventArgs();
		/// Destroys ICMPEventArgs.

	std::string hostName() const;
		/// Tries to resolve the target IP address into host name.
		/// If unsuccessful, all exceptions are silently ignored and 
		///	the IP address is returned.

	std::string hostAddress() const;
		/// Returns the target IP address.

	int repetitions() const;
		/// Returns the number of repetitions for the ping operation.

	int dataSize() const;
		/// Returns the packet data size in bytes.

	int ttl() const;
		/// Returns time to live.

	int sent() const;
		/// Returns the number of packets sent.

	int received() const;
		/// Returns the number of packets received.

	int replyTime(int index = -1) const;
		/// Returns the reply time for the request specified with index.
		/// If index == -1 (default), returns the most recent reply time.

	const std::string& error(int index = -1) const;
		/// Returns the error string for the request specified with index.
		/// If index == -1 (default), returns the most recent error string.

	int minRTT() const;
		/// Returns the minimum round trip time for a sequence of requests.

	int maxRTT() const;
		/// Returns the maximum round trip time for a sequence of requests.

	int avgRTT() const;
		/// Returns the average round trip time for a sequence of requests.

	float percent() const;
		/// Returns the success percentage for a sequence of requests.

private:
	ICMPEventArgs();

	void setRepetitions(int repetitions);
	void setDataSize(int dataSize);
	void setTTL(int ttl);
	void setReplyTime(int index, int time);
	void setError(int index, const std::string& text);
	ICMPEventArgs& operator ++ ();
	ICMPEventArgs operator ++ (int);
	static int zeroVal(int n);

	SocketAddress _address;
	int _sent;
	int _dataSize;
	int _ttl;
	std::valarray<int> _rtt;
	std::vector<std::string> _errors;

	friend class ICMPClient;
};


//
// inlines
//
inline int ICMPEventArgs::repetitions() const
{
	return (int) _rtt.size();
}


inline void ICMPEventArgs::setDataSize(int dataSize)
{
	_dataSize = dataSize;
}


inline int ICMPEventArgs::dataSize() const
{
	return _dataSize;
}


inline void ICMPEventArgs::setTTL(int ttl)
{
	_ttl = ttl;
}


inline int ICMPEventArgs::ttl() const
{
	return _ttl;
}


inline int ICMPEventArgs::sent() const
{
	return _sent;
}


inline int ICMPEventArgs::minRTT() const
{
	return _rtt.min();
}


inline int ICMPEventArgs::maxRTT() const
{
	return _rtt.max();
}


} } // namespace Poco::Net


#endif
