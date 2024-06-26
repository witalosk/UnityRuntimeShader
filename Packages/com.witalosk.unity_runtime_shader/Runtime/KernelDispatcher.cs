using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace UnityRuntimeShader
{
    public class KernelDispatcher : NativeShaderExecutorBase
    {
        public override string ShaderCode { get => _computeShaderCode; set => _computeShaderCode = value; }

        protected override IntPtr PluginCompileShaderFromString(IntPtr shaderCode) => Plugin.CompileComputeShaderFromString(_instanceId, shaderCode);
        protected override void PluginSetTexture(int slot, IntPtr texture, int format) => Plugin.SetTextureToCs(_instanceId, slot, texture, format);
        protected override void PluginSetBuffer(int slot, IntPtr buffer, int count, int stride) => Plugin.SetBufferToCs(_instanceId, slot, buffer, count, stride);
        protected override void PluginSetConstantBuffer(int slot, IntPtr buffer, int size) => Plugin.SetConstantBufferToCs(_instanceId, slot, buffer, size);

        [Space]
        [SerializeField, HideInInspector]
        private string _computeShaderCode = @"[numthreads(256, 1, 1)]
void Main (uint3 id : SV_DispatchThreadID)
{
    // Do something
}
";
        private Vector3Int _numThreads = Vector3Int.zero;
        
        private void Awake()
        {
            _instanceId = Plugin.CreateDispatcher();
            
            if (!CompileShader(out string error))
            {
                Debug.LogError(error);
            }
        }

        protected override void OnDestroy()
        {
            Plugin.ReleaseDispatcher(_instanceId);
            base.OnDestroy();
        }

        /// <summary>
        /// Compile compute shader.
        /// </summary>
        /// <param name="error">Error message.</param>
        /// <returns>True if the shader is compiled successfully.</returns>
        public override bool CompileShader(out string error)
        {
            string shaderCode = ShaderPrecompileProcessor.RemoveComments(ShaderCode);
            _numThreads = ShaderPrecompileProcessor.GetKernelThreadGroupSizes(shaderCode);
            return base.CompileShader(out error);
        }

        /// <summary>
        /// Set writable buffer to compute shader.
        /// </summary>
        /// <param name="slot">Buffer slot. In the shader, it is declared as RWStructuredBuffer&lt;T&gt; _Buffer : register(u[slot]);</param>
        /// <param name="buffer">Buffer to set.</param>
        public void SetRwBuffer(int slot, GraphicsBuffer buffer)
        {
            if (_instanceId < 0) return;
            Plugin.SetRwBufferToCs(_instanceId, slot, buffer.GetNativeBufferPtr(), buffer.count, buffer.stride);
        }
        
        /// <summary>
        /// Dispatch compute shader with the specified thread group sizes.
        /// </summary>
        public void Dispatch(int x, int y = 1, int z = 1)
        {
            if (_instanceId < 0) return;
            Plugin.Dispatch(_instanceId, x, y, z);
        }

        /// <summary>
        /// Dispatch compute shader with the desired thread num.
        /// </summary>
        public void DispatchDesired(int x, int y = 1, int z = 1)
        {
            if (_instanceId < 0) return;
            Plugin.Dispatch(_instanceId, Mathf.CeilToInt(x / (float)_numThreads.x), Mathf.CeilToInt(y / (float)_numThreads.y), Mathf.CeilToInt(z / (float)_numThreads.z));
        }
    }
}