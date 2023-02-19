#include <d3d12.h>
#include <dxgi.h>
import Memory;
import Sync;
export module Graphics;

export namespace Graphics {
    struct RenderContext;
    struct Context {
		int FrameBufferCount;
		HWND Window;
        ImmidiateValue<UINT64> FenceValue;
        ref<RenderContext> RenderContext;

        Context(int FrameBufferCount, HWND WindowHandle) :
            FrameBufferCount(FrameBufferCount), 
            FenceValue(new ref<UINT64>[FrameBufferCount]),
            Window(WindowHandle)
        { }
	};
    struct RenderContext {
		ref<IDXGISwapChain> SwapChain;
		ref<ID3D12Device> Device;
		ref<ID3D12CommandQueue> CommandQueue;
		ref<ID3D12DescriptorHeap> RTVDescriptorHeap;
		ImmidiateValue<ID3D12Resource> RenderTargets;
        ImmidiateValue<ID3D12CommandAllocator> CommandAllocator;
		ref<ID3D12GraphicsCommandList> CommandList;
        ref<ID3D12CommandAllocator> BundleCommandAllocator;
        ImmidiateValue<ID3D12Fence> Fence;
        ref<HANDLE> FenceEvent;

        RenderContext(int FrameBuffersCount) {
            RenderTargets = new ref<ID3D12Resource>[FrameBuffersCount];
            CommandAllocator = new ref<ID3D12CommandAllocator>[FrameBuffersCount];
            Fence = new ref<ID3D12Fence>[FrameBuffersCount];
        }
    };
	struct Settings {
		RECT OutputSize;
		bool FullWindow;
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
                D3D_FEATURE_LEVEL_11_0,
                IID_PPV_ARGS(&Context->Device)
            );
            /*Command Queue*/
            D3D12_COMMAND_QUEUE_DESC cqDesc = {}; // we will be using all the default values
            Context->Device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&Context->CommandQueue));
            
            /*SwapChain*/
            IDXGISwapChain* SwapChain;
            dxgiFactory->CreateSwapChain(
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
            Context->Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&Context->RTVDescriptorHeap));
            auto rtvDescriptorSize = Context->Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            auto rtvHandle(Context->RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
            for (int i = 0; i < CurrentContext->FrameBufferCount; i++){
                SwapChain->GetBuffer(i, IID_PPV_ARGS(&Context->RenderTargets[i]));
                Context->Device->CreateRenderTargetView(Context->RenderTargets[i], nullptr, rtvHandle);
                rtvHandle.ptr += rtvDescriptorSize;
            }

            //Command Allocators
            for (int i = 0; i < CurrentContext->FrameBufferCount; i++)
                Context->Device
                    ->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Context->CommandAllocator[i]));
            Context->Device
                ->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&Context->BundleCommandAllocator));
            Context->Device->
                CreateCommandList(0,
                    D3D12_COMMAND_LIST_TYPE_DIRECT,
                    Context->CommandAllocator[0],
                    NULL, IID_PPV_ARGS(&Context->CommandList));
            for (int i = 0; i < CurrentContext->FrameBufferCount; i++)
            {
                Context->Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Context->Fence[i]));
                CurrentContext->FenceValue[i] = 0;
            }
            CurrentContext->FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		}
	};
    struct Render {
        ref<Context> Context;
        void Frame() {
            auto& RenderContext = Context->RenderContext;
            auto& CommandList = RenderContext->CommandList;

            CommandList->Reset(RenderContext->CommandAllocator[1], NULL);

        }
        bool FreeNextFrame(bool ThreadLock = false) {
            const auto& RenderContext = Context->RenderContext;
            RenderContext->CommandQueue->Signal(Context->RenderContext->Fence[0], fence);
            m_fenceValue++;

            // Wait until the previous frame is finished.
            if (m_fence->GetCompletedValue() < fence)
            {
                ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
                WaitForSingleObject(m_fenceEvent, INFINITE);
            }

            m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
            return false;
        }
    };
}