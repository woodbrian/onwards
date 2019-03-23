#pragma once
//Code generated by the C++ Middleware Writer version 1.14.
inline void
base::MarshalMembers (::cmw::SendBuffer& buf)const{
}

template<class R>
base::base (::cmw::ReceiveBuffer<R>& buf)
{}

template<class C,class R>
void baseSwitch(C& c,::cmw::ReceiveBuffer<R>& buf){
  switch(auto typeNum=::cmw::Give<uint8_t>(buf);typeNum){
  case base::typeNum:
    ::cmw::BuildSegment<base>(c,buf);break;
  case derived1::typeNum:
    ::cmw::BuildSegment<derived1>(c,buf);break;
  case derived3::typeNum:
    ::cmw::BuildSegment<derived3>(c,buf);break;
  case derived2::typeNum:
    ::cmw::BuildSegment<derived2>(c,buf);break;
  default:
    throw cmw::failure("baseSwitch: Unknown type");
  }
}

inline void
derived1::MarshalMembers (::cmw::SendBuffer& buf)const{
  base::MarshalMembers(buf);
  buf.Receive(a);
}

template<class R>
derived1::derived1 (::cmw::ReceiveBuffer<R>& buf):
   base(buf),a(::cmw::Give<uint32_t>(buf))
{}

inline void
derived2::MarshalMembers (::cmw::SendBuffer& buf)const{
  base::MarshalMembers(buf);
  buf.Receive(b);
}

template<class R>
derived2::derived2 (::cmw::ReceiveBuffer<R>& buf):
   base(buf),b(::cmw::Give<uint32_t>(buf))
{}

inline void
derived3::MarshalMembers (::cmw::SendBuffer& buf)const{
  derived1::MarshalMembers(buf);
  buf.Receive(c);
}

template<class R>
derived3::derived3 (::cmw::ReceiveBuffer<R>& buf):
   derived1(buf),c(::cmw::Give<double>(buf))
{}

namespace testing{
inline void Marshal (::cmw::SendBuffer& buf
         ,::boost::base_collection<base> const& a)try{
  buf.ReserveBytes(4);
  ::cmw::MarshalCollection<base,derived1,derived3,derived2>(a,buf);
  buf.FillInSize(10000);
}catch(...){buf.Rollback();throw;}

template<class R>void Give (::cmw::ReceiveBuffer<R>& buf
         ,::boost::base_collection<base>& a){
  BuildCollection(a,buf,[](auto& a,auto& buf){baseSwitch(a,buf);});
}
}
