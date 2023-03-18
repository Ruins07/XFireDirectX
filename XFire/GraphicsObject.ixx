export module GraphicsObject;
import Memory;
import Compute;

export namespace Graphics {
	namespace Object {
		struct Effect;
		struct Material;

		struct LoadPolicy {
			bool CanBeFetched = true;
			bool Garanteed = false;
			enum Distance {
				GeneralCache = 0, //may be used
				Near = 1,
				Immidiate = 2,
				Instanced = 3
			} Distance;
			bool Time = false; //Frame or Time
			float Waiting = 1.0f; //Frames or time
		};
		struct Resource {
			ushort Ordinal;
		};
		struct Model: Resource {
			
		};
		struct Texture: Resource {
			
		};
		struct ShaderResource: Resource {
			
		};
		
		struct Effect { /*Specific structure for every shader*/ };
		struct Shader { //Shader input through uniforms
			struct EffectRecord {
				enum Resource {
					None,
					Sampler,
					ResourceView,
					UnorderedView,
					ConstantBuffer
				} Resource = Resource::None;
				enum Format {
					Boolean,
					Unsigned,
					Signed,
					Floating
				};
				short Size = 4, //negative for special values
					ComponentsCount = 1; //size of component vector size ( f.e. float = 1, float 4 = 4)
				Array<byte> Dimensions = Array<byte>();
				string Name;
			};
			virtual Array<EffectRecord> GPUProgramArgumentsInfo() { return Array<EffectRecord>(); }
			Data Source, Program;

			virtual void Input(ref<Effect> input) { }
		};
		struct EffectProvider {
			ref<Shader> Bind(){ return nullptr; }
			ushort Size() { return 0; } //Info

			void Set(ref<Effect> Effect) { Bind()->Input(Effect); }
		};
		struct MaterialType {
			Array<EffectProvider> ProvidedEffects = Array<EffectProvider>();
		};
		struct Material {
			virtual ref<MaterialType> Type() { return nullptr; }
			virtual ref<Effect> Compile(){ return nullptr; }
			
			Material(Array<ref<Effect>> Compiled){ }
			Material(){ }
		};
	}
	struct ResourceHandle {

	};
	struct ResourceDictionary {
		Array<Object::Resource> AllocatedResource;
		Array<ref<ResourceHandle>> ResourceHandle;
	};
	struct UniformBuffer {
		struct Format {
			byte BitSize;
			Compute::ValueFormat Encoding;
		};
		Array<int> DimensionSize;
		Array<Format> Format;
		int Stride = 0;
	};
	struct SpecificBuffer: UniformBuffer {
		bool Texture = false;
		int MipMapLevel = 0; //Off
	};
	struct IResourceLoader {
		virtual Object::Resource Allocate(ref<UniformBuffer> BufferInfo) {

		}
		virtual void Delete(Object::Resource Resource) {

		}
		virtual void Load(Object::Resource Dst, ref<> Src, int Size) {

		}
	};
	struct Command {
		Array<ref<Object::EffectProvider>> Shader;
		Array<ref<Object::Effect>> EffectPerModel;
		Array<Object::Model> Models;

		void SetupEffects(int Index) {
			ref<void> DataPointer;
			for (int I = 0; I < Shader.Size; I++) {
				//Shader[I]->Set((ref<Object::Effect>)DataPointer);
				//DataPointer = (ref<>)((size_t)DataPointer + Shader[I]->Size());
			}
		}
	};
}