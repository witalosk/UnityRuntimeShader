using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace RuntimeFragmentShader
{
    public abstract class NativeShaderExecutorBase : MonoBehaviour
    {
        public abstract string ShaderCode { get; set; }
        
        protected abstract IntPtr PluginCompileShaderFromString(IntPtr shaderCode);
        protected abstract void PluginSetTexture(int slot, IntPtr texture, int format);
        protected abstract void PluginSetBuffer(int slot, IntPtr buffer, int count, int stride);
        protected abstract void PluginSetConstantBuffer(IntPtr buffer, int size);
        
        protected readonly Dictionary<int, RenderTexture> _tex2dDic = new();
        protected int _instanceId = -1;
        protected IntPtr _constantBufferPtr = IntPtr.Zero;
        protected int _constantBufferSize = 0;
        
        public bool CompileShader(out string error)
        {
            if (_instanceId < 0)
            {
                error = "[ShaderRenderer] Instance is not created.";
                return false;
            }
            
            string shaderCode = ShaderPrecompileProcessor.ProcessInclude(ShaderCode);
            IntPtr result = PluginCompileShaderFromString(Marshal.StringToHGlobalAnsi($"struct VsOutput {{ float4 pos : SV_POSITION; float2 uv : TEXCOORD0; }}; {shaderCode}"));
            string resultString = Marshal.PtrToStringAnsi(result);
            if (!string.IsNullOrEmpty(resultString) || resultString != "")
            {
                error = resultString;
                return false;
            }

            error = null;
            return true;
        }
        
        public bool CompileShaderFromString(string shaderCode, out string error)
        {
            ShaderCode = shaderCode;
            return CompileShader(out error);
        }
        
        public void SetTexture(int slot, Texture texture)
        {
            if (_instanceId < 0 || texture == null) return;
            
            if (texture is RenderTexture rt)
            {
                if (_tex2dDic.TryGetValue(slot, out var renderTex))
                {
                    renderTex.Release();
                    _tex2dDic.Remove(slot);
                }
                
                if (!rt.IsCreated())
                {
                    rt.Create();
                }
                
                PluginSetTexture(slot, texture.GetNativeTexturePtr(), (int)rt.format.GetDxgiFormat());
                return;
            }

            if (texture is Texture2D t2d)
            {
                if (_tex2dDic.TryGetValue(slot, out var renderTex))
                {
                    if (renderTex.width == t2d.width && renderTex.height == t2d.height)
                    {
                        Graphics.Blit(t2d, renderTex);
                        return;
                    }
                    
                    renderTex.Release();
                }
 
                _tex2dDic[slot] = new RenderTexture(t2d.width, t2d.height, 0);
                _tex2dDic[slot].Create();
                Graphics.Blit(t2d, _tex2dDic[slot]);
                
                PluginSetTexture(slot, _tex2dDic[slot].GetNativeTexturePtr(), (int)_tex2dDic[slot].format.GetDxgiFormat());
                return;
            }

            throw new NotSupportedException("This texture type is not supported.");
        }
        
        public void SetConstantBuffer<T>(T buffer) where T : struct
        {
            if (_instanceId < 0) return;
            if (_constantBufferPtr == IntPtr.Zero || _constantBufferSize != Marshal.SizeOf<T>())
            {
                if (_constantBufferPtr != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(_constantBufferPtr);
                }
                _constantBufferPtr = Marshal.AllocHGlobal(Marshal.SizeOf<T>());
                _constantBufferSize = Marshal.SizeOf<T>();
                PluginSetConstantBuffer(_constantBufferPtr, Marshal.SizeOf<T>());
            }
            Marshal.StructureToPtr(buffer, _constantBufferPtr, _constantBufferSize > 0);
        }
        
        public void SetBuffer(int slot, GraphicsBuffer buffer)
        {
            if (_instanceId < 0) return;
            PluginSetBuffer(slot, buffer.GetNativeBufferPtr(), buffer.count, buffer.stride);
        }

    }
}