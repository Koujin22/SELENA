
#include "ModuleMsgService.hpp"

void mms::moduleSenderService(string portNumberPub, string portNumberSub, EventMods* event, function<void(string topic, string msg)>& sendMsg) {
    //----------Variables
    //loop control
    bool stop = false;
    auto stopLoop = [&stop, portNumberPub, portNumberSub](Intent* _) -> void
    {
        stop = true;
    };
    shared_ptr<void> listener = event->add("stop", stopLoop);
    //  Prepare our context
    zmq::context_t context(2);
    // Prepare our socket PUB
    zmq::socket_t socket_pub(context, zmq::socket_type::pub);
    socket_pub.bind("tcp://*:" + portNumberPub);

    // Prepare our socket SUB
    zmq::socket_t socket_sub(context, zmq::socket_type::sub);
    socket_sub.bind("tcp://*:" + portNumberSub);
    socket_sub.set(zmq::sockopt::subscribe, "");
    socket_sub.set(zmq::sockopt::rcvtimeo, 1000);

    this_thread::sleep_for(10ms);

    auto sendLocalMsg = [&socket_pub](string topic, string msg) -> void {
        zmq::message_t topic2send(topic.size());
        zmq::message_t msg2send(msg.size());

        memcpy(topic2send.data(), topic.c_str(), topic.size());
        memcpy(msg2send.data(), msg.c_str(), msg.size());
        cout << "Senidng msg to modules | " << topic2send << " | " << msg2send << endl;
        socket_pub.send(topic2send, zmq::send_flags::sndmore);
        socket_pub.send(msg2send, zmq::send_flags::none);
        cout << "Se envio!" << endl;
    };

    while (!stop) {

        zmq::message_t identity;
        zmq::message_t recv_msgs;
        zmq::recv_result_t ret = socket_sub.recv(identity, zmq::recv_flags::none);
        if (!ret.hppas_value()) {
            continue;
        }
        else {
            
            socket_sub.recv(recv_msgs, zmq::recv_flags::none);

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

            event->emit(rpl, nullptr);
        }
    }
}