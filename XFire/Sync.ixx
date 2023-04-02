export module Sync;
import Memory;

export template<typename T>
struct Handler {
	virtual void Handle(T Item);
};

export namespace ASync {
	template<typename T>
	struct DelayResult {
		virtual bool Ready();
		virtual T Wait();
		virtual T Await(bool ThisPointReturn = true);
		virtual T ASync(ref<Handler<T>> Handler);

		operator T() { return Wait(); }
	};
	template<typename T>
	using Delay = DelayResult<T>;

	struct TaskPool {
		template<typename T>
		struct Task {
			Delay<T> Item;
			bool Async = false, MultiCore = false;
		};
	};
}