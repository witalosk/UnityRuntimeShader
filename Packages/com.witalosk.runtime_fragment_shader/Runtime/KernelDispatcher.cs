using System;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.Serialization;

namespace RuntimeFragmentShader
{
    public class KernelDispatcher : NativeShaderExecutorBase
    {
        public override string ShaderCode { get => _computeShaderCode; set => _computeShaderCode = value; }
        
        [Space]
        [SerializeField, HideInInspector]
        private string _computeShaderCode = @"[numthreads(256, 1, 1)]
void Main (uint3 id : SV_DispatchThreadID)
{
    // Do something
}
";
        
        private int _instanceId = -1;
        private IntPtr _constantBufferPtr = IntPtr.Zero;
        private int _constantBufferSize = 0;

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

        public override bool CompileShader(out string error)
        {
            if (_instanceId < 0)
            {
                error = "[KernelDispatcher] Instance is not created.";
                return false;
            }
            
            string shaderCode = ShaderPrecompileProcessor.ProcessInclude(_computeShaderCode);
            IntPtr result = Plugin.CompileComputeShaderFromString(_instanceId, Marshal.StringToHGlobalAnsi(shaderCode));
            string resultString = Marshal.PtrToStringAnsi(result);
            if (!string.IsNullOrEmpty(resultString) || resultString != "")
            {
                error = resultString;
                return false;
            }

            error = null;
            return true;
        }
        
        public override bool CompileShaderFromString(string shaderCode, out string error)
        {
            _computeShaderCode = shaderCode;
            return CompileShader(out error);
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
                Plugin.SetConstantBufferToCs(_instanceId, _constantBufferPtr, Marshal.SizeOf<T>());
            }
            Marshal.StructureToPtr(buffer, _constantBufferPtr, _constantBufferSize > 0);
        }
        
        public void SetRwBuffer(int slot, GraphicsBuffer buffer)
        {
            if (_instanceId < 0) return;
            Plugin.SetRwBufferToCs(_instanceId, slot, buffer.GetNativeBufferPtr(), buffer.count, buffer.stride);
        }
        
        public void SetTexture(int slot, RenderTexture rt)
        {
            if (_instanceId < 0) return;
            if (!rt.IsCreated())
            {
                rt.Create();
            }
            Plugin.SetTextureToCs(_instanceId, slot, rt.GetNativeTexturePtr(), (int)rt.format.GetDxgiFormat());
        }
        
        public void Dispatch(int x, int y, int z)
        {
            if (_instanceId < 0) return;
            Plugin.Dispatch(_instanceId, x, y, z);
        }
    }
}