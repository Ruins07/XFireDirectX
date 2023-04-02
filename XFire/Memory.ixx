#define DEBUG

export module Memory;
export import Arithmetic;
import <CMath>;
import <initializer_list>;

export {
	using byte = unsigned char;
	using sbyte = signed char;
	using ushort = unsigned short;
	template<typename T = void>
	using ref = T*;

	constexpr int CacheLineSize = 64,
		PointersPerCacheLine = CacheLineSize / sizeof(ref<>);
	
	template<typename T>
	constexpr int ItemsPerCacheLine() {
		return CacheLineSize / sizeof(T);
	}

	template<typename T>
	struct RangeInfo {
		bool CountMode = false;
		ref<T> Begin;
		union {
			ref<T> End;
			int Count;
		};

		int CalculateSize() {
			if (CountMode) return Count;
			else return End - Begin;
		}

		ref<T> CalculateEnd() {
			if (CountMode) return Begin + Count;
			else return End;
		}

		RangeInfo(ref<T> Begin, ref<T> End)
			: Begin(Begin), End(End), CountMode(false) { }
		RangeInfo(ref<T> Begin, int Count)
			: Begin(Begin), Count(Count), CountMode(true) { }
	};


	void Move(
		__restrict ref<byte> Src, 
		__restrict ref<byte> Dst,
		int Size) {

		ref<byte>
			NewSrc = Src + Size,
			NewDst = Dst + Size;
		while(NewSrc > Src)
			*Dst-- = *Src--;
	}
	template<typename T>
	void Move(
		__restrict ref<T> Src,
		__restrict ref<T> Dst, 
		int Count) {

		Move((ref<>)Src, (ref<>)Dst, Count * sizeof(T));
	}
	template<typename T>
	void VirtualMove(
		__restrict ref<T> Src,
		__restrict ref<T> Dst, 
		int Count) {

		ref<T>
			NewSrc = Src + Count,
			NewDst = Dst + Count;
		while (NewSrc > Src)
			*Dst-- = *Src--;
	}

	template<typename T>
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
	struct Array {
		unsigned short Size;
		ref<T> Items;

		Array(int Size = 0) 
			: Size(Size){
			if (Size > 0) Items = new T[Size];
			else Items = nullptr;
		}
		Array(ref<Array> Src, int NewSize) {
			auto Border = min(Src->Size, NewSize);
			Items = new T[NewSize];
			Move<T>(Src->Items, Items, Border);
		}
		~Array() {
			delete Items;
		}
		T& Item(int Index) {
			return Items[Index];
		}
		T& operator[](int Index) {
			return Item(Index);
		}
		bool Empty() { return Size != 0; }
	};
	using string = ref<Array<char>>;
	using Data = Array<byte>;

	template<typename T>
	struct Entry: Array<T> {
		Entry(int Size = 0)
			: Array<T>(0) {
		}
		Entry(ref<T> Item) {
			this->Items = Item;
			this->Size = 1;
		}
		operator T() {
			#ifdef DEBUG
			if (!One()) throw;
			#endif
			return this[0];
		}
		bool One() { return this->Size == 1; }
	};
}