export module Pool;
import Memory;
import Sequence;

export template<typename T>
struct Pool {
	struct Handler { //DifferedHandler
		Sequence<T> AdditionItems = Sequence<T>();
		Sequence<int> RemoveIndeces = Sequence<int>();
		Sequence<RangeInfo> RemoveRanges = Sequence<RangeInfo>();

		Sequence<ref<Entry<T>>> AdditionEntries = Sequence<ref<Entry<T>>>();
		Sequence<ref<Entry<int>>> RemoveEntries = Sequence<ref<Entry<int>>>();
		Sequence<ref<Entry<RangeInfo>>> RemoveRangeEntries = Sequence<ref<Entry<RangeInfo>>>();

		void Add(Entry<T> Items) {
			AdditionItems.Add(Items);
		}
		void Add(Entry<ref<Entry<T>>> ItemReference) {
			AdditionEntries.Add(ItemReference);
		}
		void Remove(Entry<int> Indeces) {
			RemoveIndeces.Add(Indeces);
		}
		void Remove(Entry<ref<Entry<int>>> IndecesReference) {
			RemoveEntries.Add(IndecesReference);
		}
		void Remove(Entry<RangeInfo> Range) {
			RemoveRanges.Add(Range);
		}
		void Remove(Entry<ref<Entry<RangeInfo>>> RangeReference) {
			RemoveRangeEntries.Add(RangeReference);
		}

		int TotalAddition() {
			auto Counter = AdditionItems.Count;
			for (int I = 0; I < AdditionEntries.Count; I++)
				Counter += AdditionEntries.Count;
			return Counter;
		}
		int TotalRemove() {

		}
		int TotalChange() {
			return TotalAddition() - TotalRemove();
		}
	};
	struct CopyPass {
		ref<T> Src, Dst;
		const int Size = 0;
		int Cursor = 0;

		CopyPass(ref<T> Src,
				 ref<T> Dst,
				 int Size)
			: Src(Src), Dst(Dst), Size(Size){ }

		bool ContinueCopy(int UntilIndex = -1) { //-1 mean until end cause Cursor already passed
			//return Cursor < Size;
			if (UntilIndex > Cursor
				&& UntilIndex < Size) {
				auto PartLengh = UntilIndex - Cursor;
				if (PartLengh > 0) {
					Move<T>(Src, Dst, PartLengh);
					Src += PartLengh;
					Dst += PartLengh;
					Cursor += PartLengh;
				}
				return true;
			}
			else { //Until end
				#ifdef DEBUG
				if (UntilIndex >= 0 
					&& UntilIndex < Cursor) throw;
				#endif
				auto LastPart = Size - Cursor;
				Move<T>(Src, Dst, LastPart);
				Cursor += LastPart;
				Src += LastPart;
				Dst += LastPart;
				return false;
			}
		}
		void Write(Entry<T> Item) {
			Move<T>(Item.Items, Dst, Item.Size);
			Cursor += Item.Size;
			Src += Item.Size;
			Dst += Item.Size;
		}
	};

	int Count;
	Array<T> Storage;

	void Add(Entry<T> Item) {
		auto NewSize = Item.Size - Reserved();
		if (NewSize > 0) Reserve(NewSize);
		Move<T>(Item.Items, &Storage[Count], Item.Size);
	}
	void Remove(Entry<int> Index, bool Sorted = true) {
		int TotalLost = 0;
		if (Sorted) {
			for (int I = Index.Size - 1; I > 0; I--) {
				TotalLost++;
				if (Index[I] < Count - 1 - Index.Size) break;
			}
			for (int I = 0, EndOffset = 0;
				I < Index.Size - TotalLost; I++) {
				auto CurrenEndI = Count - 1 - I;
				while (Index[EndOffset] == CurrenEndI
					- EndOffset)
					EndOffset++;

				Storage[I] = Storage[CurrenEndI - EndOffset];
			}
		}
		else {
			int LostIndeces[Index.Size];
			for (int I = Index.Size - 1; I > 0; I--)
				if (Index[I] >= Count
					- 1
					- Index.Size) {
					TotalLost++;
					LostIndeces[TotalLost] = Index[I];
				}
			for (int I = 0, EndOffset = 0;
				I < Index.Size - TotalLost; I++) {
				auto CurrenEndI = Count - 1 - I;
				bool Found = false;
				do {
					for (int CurrentIndex = 0; 
						CurrentIndex < TotalLost;
						CurrentIndex++)

						if (LostIndeces[CurrentIndex] 
							== CurrenEndI - EndOffset) {
							Found = true;
							EndOffset++;
							break;
						}
				} while (Found);

				Storage[I] = Storage[CurrenEndI - EndOffset];
			}
		}
	}
	void Handle(Handler Handler) {
		
	}
	int Reserved() {
		return Storage->Size - Count;
	}
	void Reserve(int Count) {
		auto NewStorage = Array(&Storage, Storage.Size + Count);
		~Storage();
		Storage = NewStorage;
	}
	void Cull(int Count) {
		auto NewStorage = Array(&Storage, Storage.Size - Count);
		~Storage();
		Storage = NewStorage;
	}
};