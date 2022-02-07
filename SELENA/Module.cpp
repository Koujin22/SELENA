
#include "Module.hpp"
#include "PythonService.hpp"
#include <chrono>

Module::Module(ModPlatform platformType, string nameOfFile, string identifier, ModulesResources* modRsrc, bool debug = false)
{

    id = identifier;
    arguments = "50005 50004 -d=";
    arguments.append(((debug_f) ? "true" : "false"));
    arguments.append(" -n=");
    arguments.append(id);

    fileName = nameOfFile;
    debug_f = debug;

    platform = platformType;

    aliveListener = modRsrc->addEventListener(id + "|proofOfLife", [this](Intent* _)->void {
        {
            this->signal.notify_one();
        }
    });
}

void Module::startProcess(FrameworkResources* fwRsrc)
{

    if (platform == ModPlatform::python) {
        isAliveFn = pys::startProcess(fileName + " " + arguments, debug_f, fwRsrc, processStop);
    }
    else {
        throw "Platform not supported";
    }
    auto timeout = chrono::system_clock::now() + chrono::seconds(5);
    {
        unique_lock<mutex> lock(lockObj);
        cv_status status = signal.wait_until(lock, timeout);
        if (status != cv_status::no_timeout) {
            throw "Couldnt launch module with identity";
        }
    }
}

void Module::addEventToken(token evntToken)
{
    eventListeners.push_back(evntToken);
}

bool Module::isAlive()
{
    if (isAliveFn == nullptr)return false;
    else return (*isAliveFn)();
}

Module::~Module()
{
    delete isAliveFn;
}
