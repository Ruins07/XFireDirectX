import <CMath>;
export module Memory;

export {
	using byte = unsigned char;
	using sbyte = signed char;
	using ushort = unsigned short;
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
		short Min = 0, Max = 0x7FFF, Cursor = 0;
		void Inc(int Step = 1) {
			Cursor += Step;
			auto DistanceToMax = Max - Cursor;
			if (DistanceToMax <= 0) Cursor = Min + abs(DistanceToMax);
		}
		void Dec(int Step = 1) {
			Cursor -= Step;
			auto DistanceToMin = Cursor - Min;
			if (DistanceToMin <= 0) Cursor = Max - abs(DistanceToMin);
		}
	};
	template<typename T>
	struct IPass {
		virtual ref<T> Item(int Step = 1, bool Next = false);
		virtual void Next(int Skip = 1);
	};
	template<typename T>
	struct Array {
		unsigned short Size;
		ref<T> Items;

		Array(int Size = 0) 
			: Size(Size){
			if (Size > 0) Items = new T[Size];
			else Items = nullptr;
		}
		T& Item(int Index) {
			return Items[Index];
		}
		T& operator[](int Index) {
			return Item(Index);
		}
		bool Empty() { return Size != 0; }
	};
	template<typename T>
	struct CountableArray: Array<T> {
		int Count = 0;
	};
	using string = ref<Array<char>>;
	using Data = Array<byte>;
}