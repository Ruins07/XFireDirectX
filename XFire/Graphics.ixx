#include <d3d12.h>
#include <dxgi.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

export module Graphics;
import Memory;
import Sync;

export namespace Graphics {
    struct RenderContext {
        CountableArray<bool> CPUFramesReady, GPUFramesClear;
        ref<IDXGISwapChain> SwapChain;
        ref<ID3D12Device> Device;
        ref<ID3D12CommandQueue> CommandQueue;
        ref<ID3D12DescriptorHeap> RTVDescriptorHeap; //Render Target View
        ref<ID3D12CommandAllocator> BundleCommandAllocator;

        struct Frame {
            ref<ID3D12Resource> RenderTarget;
            Array<ref<ID3D12GraphicsCommandList>> CommandList;
            Array<ref<ID3D12CommandAllocator>> CommandAllocator;
            ref<ID3D12Fence> CommandBufferCompleted;
            ref<UINT64> FenceCounter;
            HANDLE FenceEvent;

            Frame(int CommandListCount = 1) {
                CommandList = Array<ref<ID3D12GraphicsCommandList>>(CommandListCount);
                CommandAllocator = Array<ref<ID3D12CommandAllocator>>(CommandListCount);
            }
        };
        Array<Frame> Frames;

        RenderContext(int FrameCount = 1, int CommandListCount = 1) {
            Frames = Array<Frame>(FrameCount);

            for (int I = 0; I < Frames.Size; I++) {
                Frames[I] = Frame(CommandListCount);
            }
        }
    };
    struct Context {
		int FrameBufferCount;
		HWND Window;
        ref<RenderContext> RenderContext;

        Context(int FrameBufferCount, HWND WindowHandle) :
            FrameBufferCount(FrameBufferCount),
            Window(WindowHandle),
            RenderContext(new Graphics::RenderContext(FrameBufferCount))
        { }
	};
    
	struct Settings {
		RECT OutputSize;
		bool
            FullWindow, 
            TrippleBuffering, 
            MaximumFrameLatency;
        struct EncodingInfo {
            byte 
                ChannelMaximumBitDensity = 8, // Most bit density value
                ChannelsCount = 3;
            bool
                BitDensityEquals = true;
            struct Channel {
                byte
                    Index = 0,
                    BitDensity = 8;
                bool
                    Precomputed = false,
                    Usage = true;
            };
        };
	};
	struct Initiator {
	    ref<Context> Initialize(ref<Settings> Settings, ref<Context> CurrentContext) {
			ref<RenderContext> Context = new Graphics::RenderContext(CurrentContext->FrameBufferCount);

            IDXGIFactory* dxgiFactory;
            if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)))) throw;

            DXGI_MODE_DESC backBufferDesc = {};
                backBufferDesc.Width = Settings->OutputSize.right;
                backBufferDesc.Height = Settings->OutputSize.bottom;
                backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            DXGI_SAMPLE_DESC sampleDesc = {};
                sampleDesc.Count = 1;

            DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
                swapChainDesc.BufferCount = CurrentContext->FrameBufferCount;
                swapChainDesc.BufferDesc = backBufferDesc;
                swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                swapChainDesc.OutputWindow = CurrentContext->Window;
                swapChainDesc.SampleDesc = sampleDesc;
                swapChainDesc.Windowed = !Settings->FullWindow;
            D3D12CreateDevice(
                NULL,
                D3D_FEATURE_LEVEL_12_0,
                IID_PPV_ARGS(&Context->Device)
            );
            /*Command Queue*/
            D3D12_COMMAND_QUEUE_DESC cqDesc = {}; // we will be using all the default values
            Context->Device->
                CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&Context->CommandQueue));
            
            /*SwapChain*/
            IDXGISwapChain* SwapChain;
            dxgiFactory->
                CreateSwapChain(
                    Context->CommandQueue,
                    &swapChainDesc,
                    &SwapChain
                );
            Context->SwapChain = SwapChain;

            //RTV heap
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
                rtvHeapDesc.NumDescriptors = CurrentContext->FrameBufferCount;
                rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            Context->Device->
                CreateDescriptorHeap(
                    &rtvHeapDesc, 
                    IID_PPV_ARGS(&Context->RTVDescriptorHeap)
                );
            auto rtvDescriptorSize = Context->Device->
                GetDescriptorHandleIncrementSize(
                    D3D12_DESCRIPTOR_HEAP_TYPE_RTV
                );
            auto rtvHandle(Context->RTVDescriptorHeap->
                GetCPUDescriptorHandleForHeapStart()
            );
            for (int i = 0; i < CurrentContext->FrameBufferCount; i++){
                SwapChain->
                    GetBuffer(i, 
                        IID_PPV_ARGS(&Context->Frames[i].RenderTarget)
                    );
                Context->Device->
                    CreateRenderTargetView(
                        Context->Frames[i].RenderTarget,
                        nullptr, 
                        rtvHandle
                    );
                rtvHandle.ptr += rtvDescriptorSize;
            }

            //Command Allocators
            for (int i = 0; i < CurrentContext->FrameBufferCount; i++) {
                Context->Device->
                    CreateCommandAllocator(
                        D3D12_COMMAND_LIST_TYPE_DIRECT,
                        IID_PPV_ARGS(&Context->Frames[i].CommandAllocator[0])
                    );
                Context->Device->
                    CreateCommandList(0,
                        D3D12_COMMAND_LIST_TYPE_DIRECT,
                        Context->Frames[i].CommandAllocator[0],
                        NULL,
                        IID_PPV_ARGS(&Context->Frames[i].CommandList[0])
                    );
            }
            Context->Device
                ->CreateCommandAllocator(
                    D3D12_COMMAND_LIST_TYPE_BUNDLE,
                    IID_PPV_ARGS(&Context->BundleCommandAllocator));
            
            for (int i = 0; i < CurrentContext->FrameBufferCount; i++) {
                auto& CurrentFrame = CurrentContext->RenderContext->Frames[i];
                Context->Device->
                    CreateFence(0,
                        D3D12_FENCE_FLAG_NONE,
                        IID_PPV_ARGS(&CurrentFrame.CommandBufferCompleted)
                    );
                CurrentFrame.FenceCounter = 0;
                CurrentFrame.FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            }
            CurrentContext->RenderContext = Context;
            return CurrentContext;
		}
	};
    struct Render {
        bool FullQueueWrite = false, //False for triple-buffered
            IndirectRender = true;
        ref<Context> Context;
        void Frame(int Number = 0) {
            auto& RenderContext = Context->RenderContext;
            auto& FrameContext = RenderContext->Frames[Number];
            
            RenderContext->GPUFramesClear[Number] = false;
            RenderContext->GPUFramesClear.Count--;

            FrameContext.CommandAllocator[0]->Reset();
            FrameContext.CommandList[Number]->Reset(FrameContext.CommandAllocator[0], NULL);


        }
        void FrameReady(int FrameNumber = 0) {
            Context->RenderContext->GPUFramesClear[FrameNumber] = true;
            Context->RenderContext->GPUFramesClear.Count++;
        }
    };
}