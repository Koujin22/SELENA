#include "MessageService.hpp"
#include "util.hpp"
using namespace std;

void mgs::messageReqRes(string portNumber, Event* event, function<string (string msg)>& comunicate, zmq::socket_t* socket ) {

    //loop control
    bool stop = false;

    auto stopLoop = [&stop, &portNumber]() -> void
    {

        stop = true;
    };

    //  Prepare our context and socket
    zmq::context_t context(2);
    socket = new zmq::socket_t(context, zmq::socket_type::req);
    socket->bind("tcp://*:" + portNumber);
    thread::id this_id = std::this_thread::get_id();

    event->add("stop", stopLoop);

    auto com = [&socket](string msg) -> string {
        zmq::message_t reply(msg.size());
        memcpy(reply.data(), msg.data(), msg.size());
        socket->send(reply, zmq::send_flags::none);

        zmq::message_t request;
        socket->recv(request, zmq::recv_flags::none);
        string rpl = std::string(static_cast<char*>(request.data()), request.size());
        return rpl;
    };

    comunicate = com;

}

void mgs::messageSender(string portNumber, Event* event, function<void(string msg)>& sendMsg, zmq::socket_t* socket) {

    //loop control
    bool stop = false;

    auto stopLoop = [&stop, &portNumber]() -> void
    {
        stop = true;
    };

    shared_ptr<void> listener  = event->add("stop", stopLoop);
    //  Prepare our context and socket
    zmq::context_t context(2);
    socket = new zmq::socket_t(context, zmq::socket_type::pub);
    socket->bind("tcp://*:"+portNumber );
    this_thread::sleep_for(2000ms);

    auto sendLocalMsg = [&socket](string msg) -> void {
        zmq::message_t msg2send(msg.size());
        memcpy(msg2send.data(), msg.c_str(), msg.size());

        socket->send(msg2send, zmq::send_flags::none);
    };

    sendMsg = sendLocalMsg;
    
    while (!stop) {

    }
}

void mgs::createMessageService(string portNumber, Event* event, function<void(string)>& sendMsg, zmq::socket_t* socket) {
    //loop control
    bool stop = false;
    thread::id this_id = std::this_thread::get_id();

    auto stopLoop = [&stop, portNumber]() -> void
    {
        stop = true;
    };

    //  Prepare our context and socket
    zmq::context_t context(2);
    socket = new zmq::socket_t(context, zmq::socket_type::router);
    socket->bind("tcp://*:" + portNumber);


    //setting listener for stop
    shared_ptr<void> listener = event->add("stop", stopLoop);

    auto sendLocalMsg = [&socket](string msg) -> void {
        string id = "helper";

        zmq::message_t identity(id.size());
        zmq::message_t msg2send(msg.size());

        memcpy(identity.data(), id.data(), id.size());
        memcpy(msg2send.data(), msg.c_str(), msg.size());

        socket.send(identity, zmq::send_flags::sndmore);
        socket.send(msg2send, zmq::send_flags::none);
    };

    sendMsg = sendLocalMsg;
    while (!stop) {
        zmq::message_t identity;
        zmq::message_t recv_msgs;

        socket->recv(identity, zmq::recv_flags::none);
        socket->recv(recv_msgs, zmq::recv_flags::none);

        string rpl;
        if (!recv_msgs.empty() && !identity.empty()) {
            cout << "Identity: " << identity << " | Msg: " << recv_msgs << endl;
            rpl = string(static_cast<char*>(recv_msgs.data()), recv_msgs.size());
        }
        else if (!identity.empty()) {
            cout << "Identity: " << identity << " | Msg: was enpty " << endl;
            rpl = string(static_cast<char*>(identity.data()), identity.size());
        }
        else
            throw "WTF";

        cout << "Msg: " << rpl << endl;


        event->emit(rpl);

    }
}
