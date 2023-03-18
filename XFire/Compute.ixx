export module Compute;

export namespace Compute {
	struct Instruct {
		enum Operation {
			Add, Sub,
			Mul, Div, Mod,
			Pow, Log
		} Operation;
	};

	enum ValueFormat {
		Boolean,
		Unsigned,
		Signed,
		Floating
	};
}