#ifndef KT_ASYNC_WORKERTHREAD_H_
#define KT_ASYNC_WORKERTHREAD_H_

/**
 * WORKER-THREAD and OPERATOR-THREAD
 * These classes manage a worker thread and send operations to be performed on that
 * thread. The difference is that OperatorThread also memory manages the operations,
 * making it the most convenient and preferred class.
 *
 * The design is intended to force data to only exist in a single thread at a time:
 * It's allocated in the main thread, then moved to the worker thread, then moved
 * back, so clients don't need to do any locking.
 */

#include <atomic>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace kt {
namespace async {

/**
 * @class kt::async::WorkerThread
 * @brief Handle running an operation in a separate thread. Assume a basic pattern of
 * supplying an IO (input/output) object, running it in the thread, and returning it as an output object.
 ** IMPLICIT INTERFACE
 *		// For both functions, ThreadData is the TD template parameter
 *		// replace() is an optimization -- ThisClass is a different instance
 *		// of the IO class. If you return true, then run() will not be called on ThisClass.
 *		// ThisClass will always be an earlier operation in the list.
 *		bool		replace(ThisClass&, ThreadData&) { return false; }
 *		void		run(ThreadData&)
 */
// IO is the input/output operation class, TD is the thread data. If you don't want any, use int.
template <typename IO, typename TD>
class WorkerThread {
public:
	// Set a handler to receive notification when an IO operation is complete. This is
	// always called from the main thread. Clients can take ownership of the ptr if they want.
	WorkerThread(	const std::function<void(std::unique_ptr<IO>&)> &handler_fn,
					const std::string &name);
	~WorkerThread();

	void								run(std::unique_ptr<IO>);
	void								update();

private:
	WorkerThread();
	WorkerThread(const WorkerThread<IO, TD>&);
	WorkerThread&						operator=(const WorkerThread<IO, TD>&);
	void								loop();

	const std::function<void(std::unique_ptr<IO>&)>
										mHandlerFn;

	const std::string					mName;
	std::thread							mThread;
	std::atomic_bool					mStop;
	std::mutex							mConditionMutex,
										mTransportMutex;
	std::condition_variable				mCondition;

	std::deque<std::unique_ptr<IO>>		mInput;
	std::vector<std::unique_ptr<IO>>	mOutput,
										mHandleOutput;
};

/**
 * @class kt::async::OperatorThread
 * @brief Wrap a WorkerThread in something that caches and recycles the operations.
 */
// IO is the input/output operation class, TD is the thread data. If you don't want any, use int.
template <typename IO, typename TD>
class OperatorThread {
public:
	// Set a handler to receive notification when an IO operation is complete.
	OperatorThread(	const std::function<void(IO&)> &handler_fn,
					const std::string &name = "");

	// A new IO is generated automatically; the start_fn can be used to initialize it
	// before it goes off to the thread to run.
	void								run(const std::function<void(IO&)> &start_fn = nullptr);
	void								update() { mLoop.update(); }

private:
	void								finished(std::unique_ptr<IO>&);

	OperatorThread();
	OperatorThread(const OperatorThread<IO, TD>&);
	OperatorThread&						operator=(const OperatorThread<IO, TD>&);

	const std::function<void(IO&)>
										mHandlerFn;

	WorkerThread<IO, TD>				mLoop;
	std::vector<std::unique_ptr<IO>>	mRetired;
};

/**
 * IMPLEMENTATION - WorkerThread
 */
template <typename IO, typename TD>
WorkerThread<IO, TD>::WorkerThread(	const std::function<void(std::unique_ptr<IO>&)> &handler_fn,
									const std::string &name)
		: mName(name)
		, mHandlerFn(handler_fn) {
	mStop.store(false);
	mThread = std::thread([this](){loop();});
}

template <typename IO, typename TD>
WorkerThread<IO, TD>::~WorkerThread() {
	try {
		mStop.store(true);
		mCondition.notify_all();
		mThread.join();
	} catch (std::exception const&) {
	}
}

template <typename IO, typename TD>
void WorkerThread<IO, TD>::run(std::unique_ptr<IO> io) {
	if (!io) return;
	try {
		{
			std::lock_guard<std::mutex> lock(mTransportMutex);
			mInput.push_back(std::move(io));
		}
		mCondition.notify_all();
	} catch (std::exception const&) {
	}
}

template <typename IO, typename TD>
void WorkerThread<IO, TD>::update() {
	mHandleOutput.clear();
	{
		std::lock_guard<std::mutex> lock(mTransportMutex);
		mHandleOutput.swap(mOutput);
	}
	for (auto& e : mHandleOutput) {
		if (e && mHandlerFn) mHandlerFn(e);
	}
}

template <typename IO, typename TD>
void WorkerThread<IO, TD>::loop() {
	TD									thread_data;
	std::deque<std::unique_ptr<IO>>		input;
	while (!mStop.load()) {
		// Get input
		{
			std::lock_guard<std::mutex> lock(mTransportMutex);
			input.swap(mInput);
		}

		// Process and push to output
		{
			while (!input.empty()) {
				std::unique_ptr<IO>		op(std::move(input.front()));
				input.pop_front();
				if (input.empty() || !input.front()->replace(*(op.get()), thread_data)) {
					try {
						op->run(thread_data);
					} catch (std::exception const&) {
					}
					std::lock_guard<std::mutex> lock(mTransportMutex);
					try {
						mOutput.push_back(std::move(op));
					} catch (std::exception const&) {
					}
				}
			}
			input.clear();
		}

		// Wait
		if (mStop.load()) break;
		std::unique_lock<std::mutex>	lock(mConditionMutex);
		bool							needs_wait(true);
		{
			std::lock_guard<std::mutex> lock(mTransportMutex);
			needs_wait = mInput.empty();
		}
	    if (needs_wait) {
			mCondition.wait(lock);
		}
	}
}

/**
 * IMPLEMENTATION - OperatorThread
 */
template <typename IO, typename TD>
OperatorThread<IO, TD>::OperatorThread(	const std::function<void(IO&)> &handler_fn,
										const std::string &name)
		: mHandlerFn(handler_fn)
		, mLoop([this](std::unique_ptr<IO>& ptr){finished(ptr); }, name) {
}

template <typename IO, typename TD>
void OperatorThread<IO, TD>::run(const std::function<void(IO&)> &start_fn) {
	try {
		std::unique_ptr<IO>			ptr;
		while (!mRetired.empty()) {
			ptr = std::move(mRetired.back());
			mRetired.pop_back();
			if (ptr) break;
		}
		if (!ptr) ptr.reset(new IO());
		if (!ptr) return;
		if (start_fn) start_fn(*(ptr.get()));
		mLoop.run(std::move(ptr));
	} catch (std::exception const&) {
	}
}

template <typename IO, typename TD>
void OperatorThread<IO, TD>::finished(std::unique_ptr<IO> &ptr) {
	if (!ptr || !mHandlerFn) return;
	try {
		mHandlerFn(*(ptr.get()));
		mRetired.push_back(std::move(ptr));
	} catch (std::exception const&) {
	}
}

} // namespace async
} // namespace kt

#endif
