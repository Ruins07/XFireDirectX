export module Effect;
import Memory;

export namespace Effects {
	struct EffectDataInfo {
		byte
			Offset,
			Size; //in bytes
	};
	struct ResourceHandle { };
	struct ResourceSync {
		ref<ResourceHandle> ResourceHandle;
		virtual void Update(
			EffectDataInfo Info,
			ref<> Data) { }
	};
	struct CompiledEffectTree {
		struct EffectDataMap {
			short
				InstanceCount, //These effect used for next count of instance
				SlotCount;
		};
		struct Library {
			Array<EffectDataInfo> SlotInfo;
		} Library;
		struct CompilePool {
			struct Span {
				short Start, Size;
			};
			Array<Span> Remove;
			Array<ref<>> Add;
		};

		Array<EffectDataMap> Map;
		Array<byte> SlotInfoOrdinals;
		ref</*Effect*/> Data;

		void Reorder() {

		}

		struct EffectStream {
			struct MoveRequest {
				short Offset, Size;
				ref<> Data;
			};
			ref<CompiledEffectTree> Bind;
			int Cursor = 0,
				CurrentInstance = 0,
				CurrentMap = 0,
				CurrentSlot = 0;

			MoveRequest Next() {
				EffectDataInfo& CurrentDataInfo
					= Bind->Library.SlotInfo
					[Bind->SlotInfoOrdinals[CurrentSlot]];
				MoveRequest request = {};
				request.Offset = CurrentDataInfo.Offset;
				request.Size = CurrentDataInfo.Size;
				request.Data = (ref<>)((size_t)Bind->Data + Cursor);

				Cursor += request.Size;
				CurrentSlot++;
				if (CurrentSlot >= Bind->Map[CurrentMap]
					.SlotCount) {
					CurrentInstance++;
					CurrentSlot = 0;
				}
				if (CurrentInstance >= Bind->Map[CurrentMap]
					.InstanceCount) {
					CurrentMap++;
					CurrentInstance = 0;
				}
				return request;
			}

			EffectStream(ref<CompiledEffectTree> TreeBind)
				: Bind(TreeBind){ }
		};
	};
	/*Collection of all shaders and stages for render*/
	struct RenderPass {

	};
	struct EffectType { virtual ref<RenderPass> RenderPass() = 0; };
	struct Effect { };
	struct EffectRecord { //sometimes u may be need hold effect data with its type
		ref<EffectType> Type;
		ref<Effect> Effect;
	};
	
	struct LevelEffectLibrary {
		ref<ref<Effect>> Effect;
	};
	struct LevelEffectObject {
		int Ordinal;
	};
}