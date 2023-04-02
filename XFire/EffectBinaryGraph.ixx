export module Effect.BinaryGraph;
import Memory;

export namespace Effects {
	struct Effect { }; //Effect
	struct EffectType { }; //Effect ID
	struct EffectData { //EffectField info
		ref<EffectType> EffectType;
		byte Offset = 0,
			Size = 1;
	};
	struct ShaderInput {
		virtual ref<EffectData> EffectType() = 0;
		virtual void Load(byte DstOffset, byte Size, ref<> Src) = 0;
	};
	
	struct ObjectEffectType {
		Array<EffectData> EffectsData;
	};
	struct IObjectEffect {
		virtual ref<ObjectEffectType> EffectType() = 0;
	};
	struct EffectBinaryQueue {
		struct Transaction {
			struct Request {
				byte Size,
					DstOffset;
			};
			ushort Duration; //Number of next instances for EffectChange
			Array<Request> Requests;
		};
		void Load(Array<ref<IObjectEffect>> Effects) {

		}
		Array<Transaction> Commands;
		ref<> BinaryData;
	};//Not work
	
	struct EffectPool {

	};
}