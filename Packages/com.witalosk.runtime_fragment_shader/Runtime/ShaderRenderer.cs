using System;
using System.Collections;
using System.Runtime.InteropServices;
using UnityEngine;

namespace RuntimeFragmentShader
{
    public class ShaderRenderer : MonoBehaviour
    {
        public bool RenderEveryFrame { get; set; } = true;
        public RenderTexture TargetTexture { get => _targetTexture; set => ChangeTargetTexture(value); }

        private RenderTexture _targetTexture;
        private bool _isDestroyed = false;

        private IntPtr _instancePtr = IntPtr.Zero;
        private IntPtr _constantBufferPtr = IntPtr.Zero;
        private int _constantBufferSize = 0;

        private void Awake()
        {
            _instancePtr = Plugin.CreateRenderer();
            
            if (_targetTexture != null)
            {
                ChangeTargetTexture(_targetTexture);
            }

            StartCoroutine(OnRender());
        }

        private void OnDestroy()
        {
            _isDestroyed = true;
            Plugin.ReleaseRenderer(_instancePtr);

            if (_constantBufferPtr != IntPtr.Zero || _constantBufferSize != 0)
            {
                Marshal.FreeHGlobal(_constantBufferPtr);
            }
        }
        
        public bool CompilePixelShaderFromString(string shaderCode, out string error)
        {
            IntPtr result = Plugin.CompilePixelShaderFromString(_instancePtr, Marshal.StringToHGlobalAnsi($"struct VsOutput {{ float4 pos : SV_POSITION; float2 uv : TEXCOORD0; }}; {shaderCode}"));
            string resultString = Marshal.PtrToStringAnsi(result);
            if (!string.IsNullOrEmpty(resultString))
            {
                error = resultString;
                return false;
            }

            error = null;
            return true;
        }
        
        public void SetConstantBuffer<T>(T buffer) where T : struct
        {
            if (_constantBufferPtr == IntPtr.Zero || _constantBufferSize != Marshal.SizeOf<T>())
            {
                if (_constantBufferPtr != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(_constantBufferPtr);
                }
                _constantBufferPtr = Marshal.AllocHGlobal(Marshal.SizeOf<T>());
                _constantBufferSize = Marshal.SizeOf<T>();
                Plugin.SetConstantBuffer(_instancePtr, _constantBufferPtr, Marshal.SizeOf<T>());
            }
            Marshal.StructureToPtr(buffer, _constantBufferPtr, _constantBufferSize > 0);
        }
        
        IEnumerator OnRender()
        {
            while (!_isDestroyed)
            {
                yield return new WaitForEndOfFrame();
                
                if (!isActiveAndEnabled || !RenderEveryFrame || _targetTexture == null) continue;
                GL.IssuePluginEvent(Plugin.GetRenderEventFunc(), 1);
            }

            yield return null;
        }
        
        private void ChangeTargetTexture(RenderTexture texture)
        {
            _targetTexture = texture;
            
            if (!_targetTexture.IsCreated())
            {
                _targetTexture.Create();
            }
            
            Plugin.SetTexturePtr(_instancePtr, _targetTexture.GetNativeTexturePtr(), _targetTexture.width, _targetTexture.height, (int)_targetTexture.format.GetDxgiFormat());
        }
        
    }
}