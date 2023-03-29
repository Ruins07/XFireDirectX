export module List;
import Memory;

struct IListBase {
	unsigned short 
		Count,
		Reserved;
};
export template<typename T>
struct List: IListBase {
	struct Map {
		struct Block {
			T Items[ItemsPerCacheLine<T>()];
		};
		ref<Block> Entries[PointersPerCacheLine - 1];
		ref<Map> Next;
	};
	ref<Map> Data;

	void Reserve(int Size) {
		
	}
	void Add(Entry<T> Item) {

	}
	void Remove(Entry<int> Item) {

	}
};
export template<typename T>
struct SequenceList: IListBase {
	struct Map {
		T Entries[CacheLineSize / sizeof(ref<>) - 1];
		ref<Map> Next;
	};
	ref<Map> Data;
};