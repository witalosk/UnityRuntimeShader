using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace UnityRuntimeShader
{
    public abstract class NativeShaderExecutorBase : MonoBehaviour
    {
        public abstract string ShaderCode { get; set; }
        
        protected abstract IntPtr PluginCompileShaderFromString(IntPtr shaderCode);
        protected abstract void PluginSetTexture(int slot, IntPtr texture, int format);
        protected abstract void PluginSetBuffer(int slot, IntPtr buffer, int count, int stride);
        protected abstract void PluginSetConstantBuffer(int slot, IntPtr buffer, int size);
        
        protected readonly Dictionary<int, RenderTexture> _tex2dDic = new();
        protected readonly Dictionary<int, (int size, IntPtr ptr)> _constantBufferDic = new();
        protected int _instanceId = -1;

        protected virtual void OnDestroy()
        {
            foreach (var pair in _constantBufferDic)
            {
                if (pair.Value.ptr != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(pair.Value.ptr);
                }
            }
            
            foreach (var tex in _tex2dDic.Values)
            {
                tex.Release();
            }
        }

        /// <summary>
        /// Compile shader.
        /// </summary>
        /// <param name="error">Error message.</param>
        /// <returns>True if the shader is compiled successfully.</returns>
        public virtual bool CompileShader(out string error)
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

        /// <summary>
        /// Compile shader.
        /// </summary>
        /// <param name="shaderCode">Code to compile.</param>
        /// <param name="error">Error message.</param>
        /// <returns>True if the shader is compiled successfully.</returns>
        public bool CompileShaderFromString(string shaderCode, out string error)
        {
            ShaderCode = shaderCode;
            return CompileShader(out error);
        }
        
        /// <summary>
        /// Set texture to shader.
        /// </summary>
        /// <param name="slot">Target slot in shader. In the shader, it is declared as "Texture2D _MainTex : register(t[slot]);"</param>
        /// <param name="texture"></param>
        /// <exception cref="NotSupportedException"></exception>
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
        
        /// <summary>
        /// Set constant buffer to compute shader.
        /// </summary>
        /// <param name="slot">Target slot in Compute Buffer. In the shader, it is declared as "cbuffer cb : register(b[slot]);"</param>
        /// <param name="buffer">Buffer to set.</param>
        public void SetConstantBuffer<T>(int slot, T buffer) where T : struct
        {
            if (_instanceId < 0) return;
            int size = Marshal.SizeOf<T>();
            size = size % 16 == 0 ? size : size + 16 - size % 16;
            
            if (_constantBufferDic.TryGetValue(slot, out var data))
            {
                if (data.size == size)
                {
                    Marshal.StructureToPtr(buffer, data.ptr, size > 0);
                    return;
                }
                
                if (data.ptr != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(data.ptr);
                }
            }

            data.ptr = Marshal.AllocHGlobal(size);
            data.size = size;
            PluginSetConstantBuffer(slot, data.ptr, size);
            _constantBufferDic[slot] = data;
            
            Marshal.StructureToPtr(buffer, data.ptr, size > 0);
        }
        
        /// <summary>
        /// Set readonly buffer to compute shader.
        /// </summary>
        /// <param name="slot">Buffer slot. In the shader, it is declared as StructuredBuffer&lt;T&gt; _Buffer : register(t[slot]);</param>
        /// <param name="buffer">Buffer to set.</param>
        public void SetBuffer(int slot, GraphicsBuffer buffer)
        {
            if (_instanceId < 0) return;
            PluginSetBuffer(slot, buffer.GetNativeBufferPtr(), buffer.count, buffer.stride);
        }

    }
}