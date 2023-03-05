export module Engine;
import Memory;
import Sync;
import Graphics;

export{
	struct Engine;
	struct IEngineComponent {
		Array<ref<IEngineComponent>> Components;

		IEngineComponent(ref<Engine> BindedEngine = nullptr) { }
		virtual void Initialize() { for (int I = 0; I < Components.Size; I++) Components[I]->Initialize(); } //Game initialized but not start
		virtual void Start() { for (int I = 0; I < Components.Size; I++) Components[I]->Start(); } //Engine run
		virtual void Update() { for (int I = 0; I < Components.Size; I++) Components[I]->Update(); }
		virtual void Stop() { for (int I = 0; I < Components.Size; I++) Components[I]->Stop(); } //Engine paused
		virtual void Destroy(bool DestroyComponents = true) { //Cancel all initialized resource, save only start data, need to minimize resources
			if(DestroyComponents)
			for (int I = 0; I < Components.Size; I++) Components[I]->Destroy(); 
		}
		virtual void Unload() { for (int I = 0; I < Components.Size; I++) Components[I]->Unload();}//Full unload all component data
	};
	struct Engine: IEngineComponent {
		virtual void Initialize() override {

		}
		virtual void Start() override {

		}
		virtual void Update() override {

		}
		virtual void Stop() override {

		}
		void Render() {

		}
		virtual void Destroy(bool DestroyComponents = true) override {

		}
		void Unload() override { Destroy(); }
	};
}