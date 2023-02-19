import <CMath>;
export module Memory;

export {
	template<typename T = void>
	using ref = T*;

	template<typename T = void>
	using ImmidiateValue = ref<ref<T>>;
	
	template<typename T>
	ImmidiateValue<void> Unified(ImmidiateValue<T> Specific) {
		return const_cast<ImmidiateValue<void>>(Specific);
	}
	template<typename T>
	ref<void> Unified(ref<T> Specific) {
		return const_cast<ref<void>>(Specific);
	}
	struct RingEnumerator {
		short Min = 0, Max = 0x7FFF, I = 0;
		void Inc(int Step = 1) {
			I += Step;
			auto DistanceToMax = Max - I;
			if (DistanceToMax <= 0) I = Min + abs(DistanceToMax);
		}
		void Dec(int Step = 1) {
			I -= Step;
			auto DistanceToMax = Min + I;
			if (DistanceToMax <= 0) I = Min + abs(DistanceToMax);
		}
	};
}