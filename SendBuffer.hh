#pragma once

#include "platforms.hh"

#include "ErrorWords.hh"
#include "IO.hh"
#include "IOfile.hh"
#include "marshalling_integer.hh"
#include <stdint.h>
#include <stdio.h> // snprintf
#include <string_view>
#include <string>
#include <string.h>
#include <type_traits>

#include <limits>
static_assert(::std::numeric_limits<unsigned char>::digits==8
              ,"Only 8 bit char supported");
static_assert(::std::numeric_limits<float>::is_iec559
              ,"Only IEEE 754 supported");
#ifndef CMW_WINDOWS
#include <sys/types.h>
#include <sys/socket.h>
#endif

namespace cmw {

class SendBuffer
{
  int32_t saved_size=0;

protected:
  int index=0;
  int bufsize;
  unsigned char* buf;

public:
  sock_type sock_=-1;

  SendBuffer (unsigned char* addr,int sz) : bufsize(sz),buf(addr) {}

  void Receive (void const* data,int size);

  template<typename... T>
  void Receive_variadic (char const* format,T... t)
  {
    auto maxsize=bufsize-index;
    auto size=::snprintf(buf+index,maxsize,format,t...);
    if(size>maxsize) throw failure("SendBuffer::Receive_variadic");
    index+=size;
  }

  template <class T>
  void Receive (T val)
  {
    static_assert(::std::is_arithmetic<T>::value
                  ,"SendBuffer expecting arithmetic type");
    Receive(&val,sizeof(T));
  }

  template <class T>
  void Receive (int where,T val)
  {
    static_assert(::std::is_arithmetic<T>::value
                  ,"SendBuffer expecting arithmetic type");
    ::memcpy(buf+where,&val,sizeof(T));
  }

  void ReceiveFile (file_type fd,int32_t fl_sz)
  {
    Receive(fl_sz);
    if(fl_sz> bufsize-index)
      throw failure("SendBuffer::ReceiveFile ")<<bufsize;

    if(Read(fd,&buf[index],fl_sz)!=fl_sz)
      throw failure("SendBuffer::ReceiveFile Read");
    index+=fl_sz;
  }

  void Receive (bool b) {Receive(static_cast<unsigned char>(b));}

  void Receive (char const* cstr)
  {
    marshalling_integer slen(::strlen(cstr));
    slen.Marshal(*this);
    Receive(cstr,slen());
  }

  inline void Receive (::std::string const& s)
  {
    marshalling_integer slen(s.size());
    slen.Marshal(*this);
    Receive(s.data(),slen());
  }

  inline void Receive (::std::string_view const& s)
  {
    marshalling_integer slen(s.size());
    slen.Marshal(*this);
    Receive(s.data(),slen());
  }

  inline int GetIndex () {return index;}
  inline int ReserveBytes (int num)
  {
    if(num>bufsize-index)throw failure("SendBuffer::ReserveBytes");
    auto copy=index;
    index+=num;
    return copy;
  }

  void FillInSize (int32_t max);

  inline void Reset () {saved_size=index=0;}
  inline void Rollback () {index=saved_size;}

  template <class T>
  void ReceiveBlock (T const& grp)
  {
    int32_t count=grp.size();
    Receive(count);
    if(count>0)
      Receive(&*grp.begin(),count*sizeof(typename T::value_type));
  }

  template <class T>
  void ReceiveGroup (T const& grp,bool sendType=false)
  {
    Receive(static_cast<int32_t>(grp.size()));
    for(auto const& it:grp) it.Marshal(*this,sendType);
  }

  template <class T>
  void ReceiveGroupPointer (T const& grp,bool sendType=false)
  {
    Receive(static_cast<int32_t>(grp.size()));
    for(auto const& it:grp) it->Marshal(*this,sendType);
  }

  inline bool Flush (::sockaddr* toAddr=nullptr,::socklen_t toLen=0)
  {
    int const bytes=sockWrite(sock_,buf,index,toAddr,toLen);
    if(bytes==index){
      Reset();
      return true;
    }

    index-=bytes;
    saved_size=index;
    ::memmove(buf,buf+bytes,index);
    return false;
  }

private:
  SendBuffer (SendBuffer const&);
  SendBuffer& operator= (SendBuffer);
};
}


