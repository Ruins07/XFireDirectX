export module Sequence;
import Memory;

export template<typename T>
struct Sequence {
	struct Map {
		static const int ItemsPerMap = CacheLineSize - sizeof(ref<Map>) / sizeof(T);
		T Entries[ItemsPerMap];
		ref<Map> Next;
	};
	unsigned Count,
		Reserved;
	ref<Map> Data,
		Last;
	void Reserve() {
		auto New = new Map();
		Last->Next = New;
		Last = New;
		Reserved += Map::ItemsPerMap;
	}
	void Add(Entry<T> Item) {
		auto Size = Item.Size,
			FullSlotsCount = Count % Map::ItemsPerMap,
			FreeSlotsCount = Map::ItemsPerMap - FullSlotsCount;
		auto Src = Item.Items,
			Dst = Last->Entries;
		if (FullSlotsCount) {
			Move<T>(
				Src, 
				Last->Entries + FullSlotsCount, 
				FreeSlotsCount);
			Src += FreeSlotsCount;
			Size -= FreeSlotsCount;
		}
		while (Size > 0) {
			Reserve();
			Dst = Last;
			if (Size > Map::ItemsPerMap) {
				Move<T>(Src, Dst, Map::ItemsPerMap);
				Size -= Map::ItemsPerMap;
				Src += Map::ItemsPerMap;
			}
			else {
				Move<T>(Src, Dst, Size);
				return;
			}
		}
	}
};