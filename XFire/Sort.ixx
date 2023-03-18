export module Sort;
import Memory;

export struct Sort {
	template<typename T>
	struct Record {
		ref<T> Item;
		template<typename TValue>
		struct IPropertyProvider {
			virtual bool Number() {
				return false;
			}
			float PropertyWeight = 1.0f;

			virtual TValue ReadValue(T Item) = 0;
		};
	};
};