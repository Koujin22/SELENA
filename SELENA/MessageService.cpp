#include "MessageService.hpp"
#include "util.hpp"
#include <zmq.hpp>


using namespace std;
using namespace zmq;

void mgs::messageReqRes(string portNumber, FrameworkResources* fwRsrc ) {


    socket_t* socket = fwRsrc->addSocket(socket_type::req); 
    socket->bind("tcp://*:" + portNumber);
    
    
    thread::id this_id = std::this_thread::get_id();

    fwRsrc->setSTI(new function<string(string)>([socket](string msg) -> string {
        zmq::message_t reply(msg.size());
        memcpy(reply.data(), msg.data(), msg.size());
        socket->send(reply, zmq::send_flags::none);

        zmq::message_t request;
        socket->recv(request, zmq::recv_flags::none);
        string rpl = std::string(static_cast<char*>(request.data()), request.size());
        return rpl;
    }));

}

void mgs::messageSender(string portNumber, FrameworkResources* fwRsrc) {

    //  Prepare our context and socket
    socket_t* socket = fwRsrc->addSocket(socket_type::pub);
    socket->bind("tcp://*:"+portNumber );
    this_thread::sleep_for(2000ms);

    fwRsrc->setTTS(new function<void(string)>([socket](string msg) -> void {
        zmq::message_t msg2send(msg.size());
        memcpy(msg2send.data(), msg.c_str(), msg.size());

        socket->send(msg2send, zmq::send_flags::none);
    }));
}

void mgs::createMessageService(string portNumber, FrameworkResources* fwRsrc) {
    //loop control
    bool stop = false;
    thread::id this_id = std::this_thread::get_id();

    auto stopLoop = [&stop, portNumber]() -> void
    {
        stop = true;
    };

    socket_t* socket = fwRsrc->addSocket(socket_type::router);
    socket->bind("tcp://*:" + portNumber);
    socket->set(sockopt::rcvtimeo, 1000);


    //setting listener for stop
    fwRsrc->addEventListenerPersist("stop", stopLoop);

    fwRsrc->setWUS(new function<void(string)>([socket](string msg) -> void {
        string id = "helper";

        zmq::message_t identity(id.size());
        zmq::message_t msg2send(msg.size());

        memcpy(identity.data(), id.data(), id.size());
        memcpy(msg2send.data(), msg.c_str(), msg.size());

        socket->send(identity, zmq::send_flags::sndmore);
        socket->send(msg2send, zmq::send_flags::none);
        
    }));

    while (!stop) {
        zmq::message_t identity;
        zmq::message_t recv_msgs;

        socket->recv(identity, zmq::recv_flags::none);
        if (identity.empty()) {
            continue;
        }

        socket->recv(recv_msgs, zmq::recv_flags::dontwait);

        string rpl;
        cout << "Identity: " << identity << " | Msg: " << recv_msgs << endl;

        rpl = string(static_cast<char*>(recv_msgs.data()), recv_msgs.size());

        cout << "Msg: " << rpl << endl;

        fwRsrc->emit(rpl);

    }
}
