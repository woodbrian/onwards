#include<Buffer.hh>
#include<ErrorWords.hh>
#include<wrappers.hh>
#include"zz.frontMiddle.hh"
#include<stdio.h>
#include<stdlib.h>//exit

using namespace ::cmw;

int main (int ac,char** av){
  try{
    if(ac<3||ac>5)
      throw failure("Usage: genz account-number .mdl-file-path [node] [port]");

    windowsStart();
    getaddrinfo_wrapper res(ac<4?
#ifdef __linux__
		            "127.0.0.1"
#else
		            "::1"
#endif
                            :av[3]
                            ,ac<5?"55555":av[4],SOCK_DGRAM);
    auto r=res();
    SendBufferStack<> sbuf;
    for(;r!=nullptr;r=r->ai_next){
      if((sbuf.sock_=::socket(r->ai_family,r->ai_socktype,0))!=-1)goto sk;
    }
    if(-1==sbuf.sock_)throw failure("socket call(s) ")<<GetError();

sk: ::pollfd pfd{sbuf.sock_,POLLIN,0};
    int waitSeconds=9;
    frontMiddle::Marshal(sbuf,marshallingInt(av[1]),av[2]);
    for(int j=0;j<2;++j,waitSeconds*=2){
      sbuf.Send(r->ai_addr,r->ai_addrlen);
#ifdef __linux__
      setNonblocking(pfd.fd);
#endif
      if(pollWrapper(&pfd,1,waitSeconds*1000)>0){
        ReceiveBufferStack<SameFormat> buf(pfd.fd);
        if(buf.GiveBool())::exit(EXIT_SUCCESS);
        throw failure("cmwA:")<<buf.GiveString_view();
      }
    }
    throw failure("No reply received.  Is the cmwA running?");
  }catch(::std::exception const& e){
    ::printf("%s: %s\n",av[0],e.what());
#ifndef CMW_WINDOWS
    ::openlog(av[0],LOG_NDELAY,LOG_USER);
#endif
    syslogWrapper(LOG_ERR,"%s",e.what());
  }
  return EXIT_FAILURE;
}
