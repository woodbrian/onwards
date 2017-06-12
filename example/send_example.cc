//   The following Middle code was used as input to the C++
//   Middleware Writer.
//
//   send_example_messages
//      -out (message_id_8, ::std::vector<int32_t>, ::std::string)
//      -out (message_id_8, ::std::set<int32_t>)
//      -out (message_id_8, ::std::array<::std::array<float,2>, 3>)
//   }
//
//   receive_example_messages
//      -in                (::std::vector<int32_t>, ::std::string)
//      -in                (::std::set<int32_t>)
//      -in                (::std::array<::std::array<float,2>, 3>)
//   }

#include <getaddrinfo_wrapper.hh>
#include "message_ids.hh"
#include <platforms.hh>
#include <plf_colony.h>
#include <SendBufferStack.hh>
#include "zz.send_example_messages.hh"

#include <array>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace cmw;

int main()
{
  try{
    windows_start();
    SendBufferStack<> buffer;
    //getaddrinfo_wrapper res("127.0.0.1", "12345", SOCK_DGRAM);
    getaddrinfo_wrapper res("::1", "12345", SOCK_DGRAM);
    auto rp = res.get();
    buffer.sock_ = ::socket(rp->ai_family, rp->ai_socktype, 0);

    ::std::cout << "Enter the ID of the message to send: 1, 2, 3 or 4." << ::std::endl;
    int messageID;
    ::std::cin >> messageID;
    switch (messageID){
      case messageid1:
      {
        ::std::vector<int32_t> vec { 100, 97, 94, 91, 88, 85 };
        send_example_messages::Marshal(buffer, messageid1, vec, "Proverbs 24:27");
        break;
      }

      case messageid2:
      {
        ::std::set<int32_t> iset { 100, 97, 94, 91, 88, 85 };
        send_example_messages::Marshal(buffer, messageid2, iset);
        break;
      }

      case messageid3:
      {
        ::std::array<::std::array<float,2>, 3> ar {{ {{1.1f,2.2}}
                                                    ,{{3.3,4.4}}
                                                    ,{{5.5,6.6}}
                                                  }};
        send_example_messages::Marshal(buffer, messageid3, ar);
        break;
      }

      case messageid4:
      {
        ::plf::colony<::std::string> clny { "Beautiful words ", "wonderful words ", "of life"};
        send_example_messages::Marshal(buffer, messageid4, clny);
        break;
      }
      
      default:
        return 0;
    }
    buffer.Send(rp->ai_addr, rp->ai_addrlen);
    return 1;
  } catch (::std::exception const& ex) {
    ::std::cout << "failure: " << ex.what() << ::std::endl;
  }
}
