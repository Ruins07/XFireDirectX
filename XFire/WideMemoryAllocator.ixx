#include <intrin.h>
#pragma intrinsic(_BitScanForward)

export module WideMemoryAllocator;
export import Memory;

export struct IAllocator {
	virtual struct WideAllocation {
		ref<> Memory;
		size_t AllocatedSize;

		operator ref<>() {
			return Memory;
		}
	} Allocate(size_t Size) = 0;
	virtual void Delete(ref<> Src, size_t Size) = 0;
};

export struct HeapLocal {
	static const int HeapSize = 4 * 1024 * 1024;

	template<int HeapSize = HeapLocal::HeapSize>
	struct Map {
		struct Cell {
			byte Data;

			byte FindFreeEntry() {
				long Result;
				_BitScanForward(&Result, 0);
				return (byte)Result;
			}
		};
		struct Row {
			Cell Cells[CacheLineSize];

			byte FindCell() {
				
			}
		};

		Row Rows[HeapSize / sizeof(Row) / sizeof(Cell)];

		struct Address {
			int Row;
			byte Cell;
		} FindFreeEntry(int SequenceSize = 0) {

		}
	};

	Map<> RowsMap;
	Map<HeapSize /
		sizeof(HeapLocal::Map<>::Row) /
		sizeof(HeapLocal::Map<>::Cell)
	> MapOfMap;
};