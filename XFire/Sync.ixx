export module Sync;

export template<typename T>
struct Handler {
	virtual void Handle(T Item);
};

export namespace ASync {
	template<typename T>
	struct DelayResult {
		virtual bool Ready();
		virtual T Wait(bool ThreadWait = false);
		virtual T Await();
		virtual T ASync(Handler<T> Handler);
	};
	template<typename T>
	using Delay = DelayResult<T>;
}