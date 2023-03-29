export module Memory;
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

	void Move(ref<byte> Src, ref<byte> Dst, int Size) {
		for (int I = 0; I < Size; I++) Dst[I] = Src[I];
	}

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
	struct Array {
		unsigned short Size;
		ref<T> Items;

		Array(int Size = 0) 
			: Size(Size){
			if (Size > 0) Items = new T[Size];
			else Items = nullptr;
		}
		Array(ref<Array> Src, int NewSize) {
			Items = new T[NewSize];
			Move(Src->Items, Items, NewSize * sizeof(T));
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
			#ifdef Debug
			if (!One()) throw;
			#endif
			return this[0];
		}
		bool One() { return this->Size == 1; }
	};
	template<typename T>
	struct Pool {
		Array<T> Storage = Array<T>();
		void Add(Entry<T> Data) {
			if (Storage.Size == 0)
				Storage = Array<T>(Data.Size);
			else {
				auto OldSize = Storage.Size;
				Storage = Array<T>(Storage, OldSize + Data.Size);
				Move(Data.Items, Storage.Items[OldSize], Data.Size * sizeof(T));
			}
		}
		void Remove(Entry<int> Index, bool Sort) {
			int LastValuesCounter = 0,
				NewSize = Storage.Size - Index.Size;
			auto NewStorage = Array<T>();
			for (int I = Index.Size - 1; I > 0; I--)
				if (Index[I] >= Storage.Size - Index.Size)
					LastValuesCounter++;
			int CopyCount = Index.Size - LastValuesCounter,
				FillBegin = Storage.Size 
							- Index.Size 
							- LastValuesCounter,
				CurrentRemoveWait = 0,
				SrcOffset = 0;

			for (int I = 0; I < Storage.Size; I++) {
				if (I < CopyCount &&
					I == Index[CurrentRemoveWait]) {
					if(FillBegin + SrcOffset) 
						NewStorage[I] = Storage[SrcOffset + I];

					CurrentRemoveWait++;
					SrcOffset++;
				}
				else NewStorage[I] = Storage[SrcOffset + I];
			}

			~Storage();
			Storage = NewStorage;
		}
	};
}