using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace RuntimeFragmentShader
{
    public class KernelDispatcher : NativeShaderExecutorBase
    {
        public override string ShaderCode { get => _computeShaderCode; set => _computeShaderCode = value; }

        protected override IntPtr PluginCompileShaderFromString(IntPtr shaderCode) => Plugin.CompileComputeShaderFromString(_instanceId, shaderCode);
        protected override void PluginSetTexture(int slot, IntPtr texture, int format) => Plugin.SetTextureToCs(_instanceId, slot, texture, format);
        protected override void PluginSetBuffer(int slot, IntPtr buffer, int count, int stride) => Plugin.SetBufferToCs(_instanceId, slot, buffer, count, stride);
        protected override void PluginSetConstantBuffer(IntPtr buffer, int size) => Plugin.SetConstantBufferToCs(_instanceId, buffer, size);

        [Space]
        [SerializeField, HideInInspector]
        private string _computeShaderCode = @"[numthreads(256, 1, 1)]
void Main (uint3 id : SV_DispatchThreadID)
{
    // Do something
}
";
        
        private void Awake()
        {
            _instanceId = Plugin.CreateDispatcher();
            
            if (!CompileShader(out string error))
            {
                Debug.LogError(error);
            }
        }

        private void OnDestroy()
        {
            Plugin.ReleaseDispatcher(_instanceId);

            if (_constantBufferPtr != IntPtr.Zero || _constantBufferSize != 0)
            {
                Marshal.FreeHGlobal(_constantBufferPtr);
            }
        }
        
        public void SetRwBuffer(int slot, GraphicsBuffer buffer)
        {
            if (_instanceId < 0) return;
            Plugin.SetRwBufferToCs(_instanceId, slot, buffer.GetNativeBufferPtr(), buffer.count, buffer.stride);
        }
        
        public void Dispatch(int x, int y, int z)
        {
            if (_instanceId < 0) return;
            Plugin.Dispatch(_instanceId, x, y, z);
        }
    }
}