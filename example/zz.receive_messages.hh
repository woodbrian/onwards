#pragma once
// Code generated by the C++ Middleware Writer version 1.14.
#include<ReceiveBuffer.hh>
#include<SendBuffer.hh>
#include"message_id_types.hh"
#include<array>
#include<plf_colony.h>
#include<set>
#include<string>
#include<vector>

namespace receive_messages{
template <class R>
void Give (::cmw::ReceiveBuffer<R>& buf
         ,::std::vector<int32_t>& az1
         ,::std::string& az2){
  int32_t count[1];
  count[0]=::cmw::Give<uint32_t>(buf);
  if(count[0]>0){
    az1.resize(az1.size()+count[0]);
    buf.GiveBlock(&(*(az1.end()-count[0])),count[0]);
  }
  az2=buf.GiveString();
}

template <class R>
void Give (::cmw::ReceiveBuffer<R>& buf
         ,::std::set<int32_t>& az1){
  int32_t count[1];
  count[0]=::cmw::Give<uint32_t>(buf);
  auto endIt1=az1.end();
  for(;count[0]>0;--count[0]){
    az1.emplace_hint(endIt1,::cmw::Give<uint32_t>(buf));
  }
}

template <class R>
void Give (::cmw::ReceiveBuffer<R>& buf
         ,::std::array<::std::array<float, 2>, 3>& az1){
  buf.GiveBlock(&az1[0][0], sizeof az1 / sizeof(float));
}

template <class R>
void Give (::cmw::ReceiveBuffer<R>& buf
         ,::plf::colony<::std::string>& az1){
  int32_t count[1];
  count[0]=::cmw::Give<uint32_t>(buf);
  az1.reserve(az1.size()+count[0]);
  for(;count[0]>0;--count[0]){
    az1.insert(buf.GiveString());
  }
}
}