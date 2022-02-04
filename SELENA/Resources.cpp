#include "Resources.hpp"

template<typename T>
inline Resources<T>::Resources()
{

	context = new context_t(1);

	sockets = new vector<socket_t*>();
	threads = new vector<thread*>();
	tokens = new vector<token>();

}