export module Arithmetic;

export {
	struct Vector {
		float X, Y, Z;
	};
	template<typename T>
	struct Comparision {
		const T Min, Max;

		Comparision(T A, T B) {
			if (A > B) {
				Max = A;
				Min = B;
			}
			else {
				Min = A;
				Max = B;
			}
		}
	};
	template<typename T>
	const Comparision<T> Compare(T A, T B) {
		return Comparision(A, B);
	}
	template<typename T>
	const T Min(T A, T B) {
		return Comparision(A, B).Min;
	}
	template<typename T>
	const T Max(T A, T B) {
		return Comparision(A, B).Max;
	}
}