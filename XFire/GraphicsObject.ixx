export module GraphicsObject;
import Memory;

export namespace Graphics {
	namespace Object {
		struct Library {

		};
		struct Resource { ushort Ordinal; };
		struct Model: Resource {

		};
		struct Texture: Resource {
			
		};
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
			Array<EffectRecord> GPUProgramInput;
			Data Source, Program;
		};
		struct Effect {
			ref<Shader> Shader;
			Data ShaderInput;
		};
		struct Pass {
			Array<ref<Shader>> Shaders;
		};
		struct MaterialInfo {
			ref<Pass> Pass;
			Array<Effect> Effects;
		};
		struct Material {
			byte ID = 0;
		};
	}
}