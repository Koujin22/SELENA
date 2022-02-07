
#include "ModuleMsgService.hpp"

using namespace zmq;
void mms::moduleSenderService(string portNumberPub, string portNumberSub, ModulesResources* modRsrc) {
    //----------Variables
    //loop control
    bool stop = false;
    auto stopLoop = [&stop](Intent* _) -> void
    {
        stop = true;
    };
    modRsrc->addEventListenerPersist("stop", stopLoop);

    // Prepare our socket PUB
    socket_t* socket_pub = modRsrc->addSocket(zmq::socket_type::pub);
    socket_pub->bind("tcp://*:" + portNumberPub);

    // Prepare our socket SUB
    socket_t* socket_sub = modRsrc->addSocket(zmq::socket_type::sub);
    socket_sub->bind("tcp://*:" + portNumberSub);
    socket_sub->set(zmq::sockopt::subscribe, "");
    socket_sub->set(zmq::sockopt::rcvtimeo, 1000);

    this_thread::sleep_for(10ms);

    modRsrc->setSendToMod(new function<void(string, string)>([socket_pub](string topic, string msg) -> void {
        message_t topic2send(topic.size());
        message_t msg2send(msg.size());

        memcpy(topic2send.data(), topic.c_str(), topic.size());
        memcpy(msg2send.data(), msg.c_str(), msg.size());
        cout << "Senidng msg to modules | " << topic2send << " | " << msg2send << endl;
        socket_pub->send(topic2send, zmq::send_flags::sndmore);
        socket_pub->send(msg2send, zmq::send_flags::none);
        }));

    while (!stop) {

        message_t identity;
        message_t recv_msgs;
        recv_result_t ret = socket_sub->recv(identity, zmq::recv_flags::none);
        if (!ret.has_value()) {
            continue;
        }
        else {
            
            socket_sub->recv(recv_msgs, zmq::recv_flags::dontwait);

            string rpl;
            string id;

            cout << "Identity: " << identity << " | Msg: " << recv_msgs << endl;

            id = string(static_cast<char*>(identity.data()), identity.size());
            rpl = string(static_cast<char*>(recv_msgs.data()), recv_msgs.size());
            modRsrc->emit(id+"|"+rpl, nullptr);
        }
    }
}