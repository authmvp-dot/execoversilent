#pragma once
#include <stdexcept>
#include <imgui.h>
#include <imgui_internal.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <array>
#include <memory>
#include <functional>
#include <chrono>
#include <string>
#include <map>
#include <cstdio>

enum ImShaderTex : unsigned int { ImShaderTex_Default = 0, ImShaderTex_WindowBg, ImShaderTex_COUNT };

namespace shaderrt {

    using ImDrawFlags = int;

    inline ID3D11Device* g_device{};
    inline ID3D11DeviceContext* g_ctx{};
    inline ImColor g_main_col{};
    inline bool g_reset_time = false;

    inline float g_scale = 0.1f;
    inline float g_in_octaves = 7.f;
    inline float g_in_persistence = 10.f;

    struct VS_INPUT { DirectX::XMFLOAT3 position; };
    struct PS_INPUT { DirectX::XMFLOAT4 position; };

    inline const char* kVSH = R"(
struct VS_INPUT{ float3 position:POSITION; };
struct PS_INPUT{ float4 position:SV_POSITION; };
PS_INPUT main(VS_INPUT input){ PS_INPUT o; o.position=float4(input.position,1.0); return o; }
)";

    inline const char* kPSH = R"(
cbuffer DATA:register(b0){
  float in_time; float2 in_resolution; float in_octaves;
  float in_persistence; float in_scale; float4 in_bg_color; float4 in_fg_color; float2 align_;
}
static float4 gl_FragCoord; static float4 f_color;
struct SPIRV_Cross_Input{ float4 gl_FragCoord:SV_Position; };
struct SPIRV_Cross_Output{ float4 f_color:SV_Target0; };
void frag_main(){
  float2 uv=(2.0*gl_FragCoord.xy-in_resolution.xy)/min(in_resolution.x,in_resolution.y);
  [loop] for(int i=1;i<10;++i){ float fi=(float)i; uv.x+=0.6/fi*cos(fi*2.5*uv.y+in_time); uv.y+=0.6/fi*cos(fi*1.5*uv.x+in_time); }
  float v=0.1/abs(sin(in_time-uv.y-uv.x));
  float t=saturate(v);
  float3 rgb=lerp(in_bg_color.rgb,in_fg_color.rgb,t);
  float a=lerp(in_bg_color.a,in_fg_color.a,t);
  f_color=float4(rgb,a);
}
SPIRV_Cross_Output main(SPIRV_Cross_Input i):SV_Target{
  gl_FragCoord=i.gl_FragCoord; gl_FragCoord.w=1.0/max(gl_FragCoord.w,1e-6); frag_main();
  SPIRV_Cross_Output o; o.f_color=f_color; return o;
}
)";

    struct dx11_draw_data {
        ID3D11VertexShader* vertex_shader{};
        ID3D11InputLayout* input_layout{};
        ID3D11PixelShader* pixel_shader{};
        ID3D11Buffer* pixel_constant_buffer{};
        ID3D11BlendState* blend_state{};
    };

    struct dx11_tex {
        ID3D11Texture2D* tex{};
        ID3D11RenderTargetView* rtv{};
        ID3D11ShaderResourceView* srv{};
        ImVec2 size{};
        void release() { if (rtv) { rtv->Release(); rtv = nullptr; } if (tex) { tex->Release(); tex = nullptr; } if (srv) { srv->Release(); srv = nullptr; } }
        void create_tex() {
            size = ImGui::GetMainViewport()->Size;
            D3D11_TEXTURE2D_DESC d = {};
            d.Width = (UINT)size.x; d.Height = (UINT)size.y; d.MipLevels = 1; d.ArraySize = 1;
            d.Format = DXGI_FORMAT_R8G8B8A8_UNORM; d.SampleDesc.Count = 1;
            d.Usage = D3D11_USAGE_DEFAULT; d.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            if (FAILED(g_device->CreateTexture2D(&d, nullptr, &tex))) throw std::exception("CreateTexture2D");
        }
        void create_rtv() { if (FAILED(g_device->CreateRenderTargetView(tex, nullptr, &rtv))) throw std::exception("CreateRTV"); }
        void create_srv() { if (FAILED(g_device->CreateShaderResourceView(tex, nullptr, &srv))) throw std::exception("CreateSRV"); }
        void bind(ID3D11DeviceContext* ctx) {
            if (ImGui::GetMainViewport()->Size.x != size.x || ImGui::GetMainViewport()->Size.y != size.y) {
                release(); create_tex(); create_rtv(); create_srv();
            }
            ctx->OMSetRenderTargets(1, &rtv, nullptr);
        }
        ImTextureID texid() const { return srv; }
    };

    struct dx11_shader {
        dx11_draw_data dd{};
        dx11_tex tex;
        std::function<void(void*)> fill_cb;
        size_t cb_size{};
        static std::string get_blob_err(ID3DBlob* e) {
            std::string msg = e ? (const char*)e->GetBufferPointer() : "Unknown error";
            if (e) e->Release();
            return msg;
        }
        static void bind_vp(ID3D11DeviceContext* ctx) {
            const auto s = ImGui::GetMainViewport()->Size;
            D3D11_VIEWPORT vp{}; vp.Width = s.x; vp.Height = s.y; vp.MinDepth = 0; vp.MaxDepth = 1; vp.TopLeftX = 0; vp.TopLeftY = 0;
            ctx->RSSetViewports(1, &vp);
        }
        void make_vs(const char* src) {
            ID3DBlob* err{}, * blob{};
            if (FAILED(D3DCompile(src, strlen(src), nullptr, nullptr, nullptr, "main", "vs_4_0", 0, 0, &blob, &err))) 
                throw std::runtime_error("VS Compile Error: " + get_blob_err(err));
            if (g_device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &dd.vertex_shader) != S_OK) { 
                blob->Release(); 
                throw std::runtime_error("VS Create Error"); 
            }
            D3D11_INPUT_ELEMENT_DESC layout[] = { {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0} };
            if (g_device->CreateInputLayout(layout, 1, blob->GetBufferPointer(), blob->GetBufferSize(), &dd.input_layout) != S_OK) { 
                blob->Release(); 
                throw std::runtime_error("Input Layout Create Error"); 
            }
            blob->Release();
        }
        void make_ps(const char* src) {
            ID3DBlob* err{}, * blob{};
            if (FAILED(D3DCompile(src, strlen(src), nullptr, nullptr, nullptr, "main", "ps_4_0", 0, 0, &blob, &err))) 
                throw std::runtime_error("PS Compile Error: " + get_blob_err(err));
            HRESULT hr = g_device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &dd.pixel_shader);
            if (FAILED(hr)) { 
                blob->Release(); 
                char hr_msg[64];
                sprintf_s(hr_msg, "PS Create Error: HRESULT 0x%08X", hr);
                throw std::runtime_error(hr_msg); 
            }
            blob->Release();
        }
        void make_cb() {
            D3D11_BUFFER_DESC b{}; b.Usage = D3D11_USAGE_DYNAMIC; b.ByteWidth = (UINT)cb_size; b.BindFlags = D3D11_BIND_CONSTANT_BUFFER; b.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            if (FAILED(g_device->CreateBuffer(&b, nullptr, &dd.pixel_constant_buffer))) throw std::exception("cb");
        }
        void make_blend() {
            D3D11_BLEND_DESC d{}; d.AlphaToCoverageEnable = FALSE; d.RenderTarget[0].BlendEnable = TRUE;
            d.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; d.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; d.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            d.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; d.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA; d.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            d.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            if (FAILED(g_device->CreateBlendState(&d, &dd.blend_state))) throw std::exception("blend");
        }
        dx11_shader(const char* vsh, const char* psh, size_t cb_sz, std::function<void(void*)> cb) :fill_cb(std::move(cb)), cb_size(cb_sz) {
            make_vs(vsh); make_ps(psh); make_cb(); make_blend();
        }
        void bind(ID3D11DeviceContext* ctx) {
            tex.bind(ctx);
            bind_vp(ctx);
            ctx->IASetInputLayout(dd.input_layout);
            ctx->VSSetShader(dd.vertex_shader, nullptr, 0);
            ctx->PSSetShader(dd.pixel_shader, nullptr, 0);
            void* mem = malloc(cb_size);
            fill_cb(mem);
            D3D11_MAPPED_SUBRESOURCE m{};
            if (FAILED(ctx->Map(dd.pixel_constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m))) throw std::exception("map cb");
            memcpy(m.pData, mem, cb_size); free(mem);
            ctx->Unmap(dd.pixel_constant_buffer, 0);
            ctx->PSSetConstantBuffers(0, 1, &dd.pixel_constant_buffer);
            constexpr float bf[4] = { 0,0,0,0 };
            ctx->OMSetBlendState(dd.blend_state, bf, 0xffffffff);
        }
        ImTextureID texid() const { return tex.texid(); }
    };

    struct dx11_vbo {
        ID3D11Buffer* vb{};
        dx11_vbo() {
            struct V { DirectX::XMFLOAT3 p; };
            const V v[] = {
                {{-1.f, 1.f,0.f}}, {{ 1.f, 1.f,0.f}}, {{-1.f,-1.f,0.f}},
                {{-1.f,-1.f,0.f}}, {{ 1.f, 1.f,0.f}}, {{ 1.f,-1.f,0.f}},
            };
            D3D11_BUFFER_DESC d{}; d.Usage = D3D11_USAGE_DEFAULT; d.BindFlags = D3D11_BIND_VERTEX_BUFFER; d.ByteWidth = sizeof(v);
            D3D11_SUBRESOURCE_DATA i{}; i.pSysMem = v;
            if (FAILED(g_device->CreateBuffer(&d, &i, &vb))) throw std::exception("vb");
        }
        void draw(ID3D11DeviceContext* ctx) const {
            UINT stride = sizeof(DirectX::XMFLOAT3); UINT offset = 0;
            ctx->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
            ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            ctx->Draw(6, 0);
        }
    };

    constexpr size_t kShadersCount = ImShaderTex_COUNT;
    inline std::array<std::unique_ptr<dx11_shader>, kShadersCount> g_shaders{};
    inline std::unique_ptr<dx11_vbo> g_vbo{};

    struct alignas(16) shader_data {
        float in_time; float in_resolution[2]; float in_octaves;
        float in_persistence; float in_scale; float _pad_c1[2];
        float in_bg_color[4];
        float in_fg_color[4];
        float align_[2]; float _pad_c4[2];
    };
    static_assert(sizeof(shader_data) == 80, "bad size");
    static_assert(alignof(shader_data) == 16, "bad align");

    inline void NewFrame(IDXGISwapChain* swap_chain, ID3D11Device* device, ID3D11DeviceContext* ctx, ImColor main_color) {
        g_device = device; g_ctx = ctx; g_main_col = main_color;
        struct B {
            UINT ViewportsCount;
            D3D11_VIEWPORT Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
            ID3D11BlendState* BlendState; FLOAT BlendFactor[4]; UINT SampleMask; UINT StencilRef;
            ID3D11DepthStencilState* DepthStencilState;
            ID3D11ShaderResourceView* PSShaderResource;
            ID3D11PixelShader* PS; ID3D11VertexShader* VS;
            UINT PSInstancesCount, VSInstancesCount;
            ID3D11ClassInstance* PSInstances[256], * VSInstances[256];
            D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology;
            ID3D11Buffer* VertexBuffer, * PSConstantBuffer; UINT VertexBufferStride, VertexBufferOffset; ID3D11InputLayout* InputLayout;
        } old{};
        old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        ctx->RSGetViewports(&old.ViewportsCount, old.Viewports);
        ctx->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
        ctx->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
        ctx->PSGetShaderResources(0, 1, &old.PSShaderResource);
        old.PSInstancesCount = old.VSInstancesCount = 256;
        ctx->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
        ctx->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
        ctx->IAGetPrimitiveTopology(&old.PrimitiveTopology);
        ctx->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
        ctx->IAGetInputLayout(&old.InputLayout);

        if (!g_vbo || !g_reset_time) {
            g_vbo = std::make_unique<dx11_vbo>();
            g_shaders[ImShaderTex_Default] = std::make_unique<dx11_shader>(
                kVSH, kPSH, sizeof(shader_data),
                [](void* p) {
                    auto& d = *reinterpret_cast<shader_data*>(p);
                    const auto s = ImGui::GetMainViewport()->Size;
                    d.in_time = (float)ImGui::GetTime();
                    d.in_resolution[0] = s.x; d.in_resolution[1] = s.y;
                    d.in_octaves = 7.f; d.in_persistence = 50.f; d.in_scale = 0.15f;
                    d.in_bg_color[0] = 0.02f; d.in_bg_color[1] = 0.02f; d.in_bg_color[2] = 0.02f; d.in_bg_color[3] = 1.f;
                    d.in_fg_color[0] = 0.f; d.in_fg_color[1] = 1.f; d.in_fg_color[2] = 0.f; d.in_fg_color[3] = 1.f;
                }
            );
            g_shaders[ImShaderTex_WindowBg] = std::make_unique<dx11_shader>(
                kVSH, kPSH, sizeof(shader_data),
                [](void* p) {
                    auto& d = *reinterpret_cast<shader_data*>(p);
                    const auto s = ImGui::GetMainViewport()->Size;
                    d.in_time = (float)ImGui::GetTime() / 0.6f;
                    d.in_resolution[0] = s.x / g_in_persistence; d.in_resolution[1] = s.y / g_in_persistence / 1.2f;
                    d.in_octaves = g_in_octaves; d.in_persistence = g_in_persistence; d.in_scale = g_scale;
                    d.in_fg_color[0] = 1.f; d.in_fg_color[1] = 1.f; d.in_fg_color[2] = 1.f; d.in_fg_color[3] = 1.f;
                    d.in_bg_color[0] = 0.f; d.in_bg_color[1] = 0.f; d.in_bg_color[2] = 0.f; d.in_bg_color[3] = 1.f;
                    g_reset_time = true;
                }
            );
        }

        for (auto& s : g_shaders) { s->bind(ctx); g_vbo->draw(ctx); }

        ctx->RSSetViewports(old.ViewportsCount, old.Viewports);
        ctx->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask);
        if (old.BlendState) old.BlendState->Release();
        ctx->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef);
        if (old.DepthStencilState) old.DepthStencilState->Release();
        ctx->PSSetShaderResources(0, 1, &old.PSShaderResource);
        if (old.PSShaderResource) old.PSShaderResource->Release();
        ctx->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount);
        if (old.PS) old.PS->Release();
        for (UINT i = 0; i < old.PSInstancesCount; i++) if (old.PSInstances[i]) old.PSInstances[i]->Release();
        ctx->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount);
        if (old.VS) old.VS->Release();
        for (UINT i = 0; i < old.VSInstancesCount; i++) if (old.VSInstances[i]) old.VSInstances[i]->Release();
        ctx->IASetPrimitiveTopology(old.PrimitiveTopology);
        ctx->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
        if (old.VertexBuffer) old.VertexBuffer->Release();
        ctx->IASetInputLayout(old.InputLayout);
        if (old.InputLayout) old.InputLayout->Release();
    }

    inline ImTextureID Get(ImShaderTex s) {
        if (!g_shaders[0]) return nullptr;
        return g_shaders[s]->texid();
    }

    inline void Draw(ImDrawList* dl, ImVec2 min, ImVec2 max, float rounding, float alpha, ImShaderTex s) {
        auto* t = Get(s);
        if (!t) return;
        const auto vp = ImGui::GetMainViewport()->Size;
        const auto uv_min = ImVec2(min.x / vp.x, min.y / vp.y);
        const auto uv_max = ImVec2(max.x / vp.x, max.y / vp.y);
        const auto col = ImGui::GetColorU32(0xFFFFFFFF, alpha);
        dl->AddImageRounded(t, min, max, uv_min, uv_max, ImColor(g_main_col.Value.x, g_main_col.Value.y, g_main_col.Value.z, alpha), rounding);
    }

    inline void UI() {
        ImGui::SliderFloat("Scale", &g_scale, 0.f, 100.f);
        ImGui::SliderFloat("in_persistence", &g_in_persistence, 0.f, 100.f);
        ImGui::SliderFloat("in_octaves", &g_in_octaves, 0.f, 60.f);
        
    }

} // namespace shaderrt
