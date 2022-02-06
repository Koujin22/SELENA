#include <mutex>
#include <queue>
#include <vector>
#include <deque>
#include <future>

using namespace std;

template<typename T>
class ThreadPoolMng {
public:
	ThreadPoolMng(size_t N);

    template<typename ...Args>
    void call(T fn, Args...args);
    void abort();
    void clear();
    void finish();
    
    ~ThreadPoolMng();

private:
    mutex lockObj;
    condition_variable signal;
	deque<packaged_task<void()>> work;
	vector<future<void>> finishedThreads;

    
    void thread_task();

};


template<typename T>
inline ThreadPoolMng<T>::ThreadPoolMng(size_t N)
{
	for (std::size_t i = 0; i < N; ++i)
	{
		// each thread is a std::async running this->thread_task():
		this->finishedThreads.push_back(
			async(
				std::launch::async,
				[this] { this->thread_task(); }
			)
		);
	}
}

template<typename T>
inline void ThreadPoolMng<T>::abort()
{
	this->cancel_pending();
	this->finish();
}

template<typename T>
inline void ThreadPoolMng<T>::clear()
{
	unique_lock<std::mutex> l(this->lockObj);
	this->work.clear();
}

template<typename T>
inline void ThreadPoolMng<T>::finish()
{
	{
		std::unique_lock<std::mutex> l(this->lockObj);
		for (auto&& unused : this->finishedThreads) {
			this->work.push_back({});
		}
	}
	this->signal.notify_all();
	this->finishedThreads.clear();
}

template<typename T>
inline ThreadPoolMng<T>::~ThreadPoolMng()
{
	this->finish();
}

template<typename T>
inline void ThreadPoolMng<T>::thread_task()
{
	while (true) {
		// pop a task off the queue:
		packaged_task<void()> fn;
		{
			// usual thread-safe queue code:
			std::unique_lock<std::mutex> lock(this->lockObj);
			if (this->work.empty()) {
				this->signal.wait(lock, [&] {return !this->work.empty(); });
			}
			fn = move(this->work.front());
			this->work.pop_front();
		}
		// if the task is invalid, it means we are asked to abort:
		if (!fn.valid()) return;
		// otherwise, run the task:
		fn();
	}
}

template<typename T>
template<typename ...Args>
inline void ThreadPoolMng<T>::call(T fn, Args ...args)
{

	packaged_task<void()> p(bind(fn, args...));

	{
		std::unique_lock<std::mutex> lock(this->lockObj);
		this->work.emplace_back(std::move(p)); // store the task<R()> as a task<void()>
	}
	this->signal.notify_one(); // wake a thread to work on the task

}
