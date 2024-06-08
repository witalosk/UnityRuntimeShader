using System;
using System.Collections;
using System.Runtime.InteropServices;
using UnityEngine;

namespace RuntimeFragmentShader
{
    public class ShaderRenderer : MonoBehaviour
    {
        public bool RenderEveryFrame { get => _renderEveryFrame; set => _renderEveryFrame = value; }
        public RenderTexture TargetTexture { get => _targetTexture; set => ChangeTargetTexture(value); }
        public string FragmentShaderCode => _fragmentShaderCode;

        [SerializeField] private RenderTexture _targetTexture;
        [SerializeField] private bool _renderEveryFrame = true;
        
        [Space]
        [SerializeField, TextArea(10, 20)]
        private string _fragmentShaderCode = @"float4 Frag(VsOutput input) : SV_TARGET
{
	return float4(input.uv, 1.0 - uv.x, 1.0);
}";
        
        private int _instanceId = 0;
        private IntPtr _constantBufferPtr = IntPtr.Zero;
        private int _constantBufferSize = 0;
        private bool _isDestroyed = false;

        private void Awake()
        {
            _instanceId = Plugin.CreateRenderer();
            
            if (!CompileFragmentShader(out string error))
            {
                Debug.LogError(error);
            }
            
            if (_targetTexture != null)
            {
                ChangeTargetTexture(_targetTexture);
            }

            StartCoroutine(OnRender());
        }

        private void OnDestroy()
        {
            _isDestroyed = true;
            Plugin.ReleaseRenderer(_instanceId);

            if (_constantBufferPtr != IntPtr.Zero || _constantBufferSize != 0)
            {
                Marshal.FreeHGlobal(_constantBufferPtr);
            }
        }
        
        public bool CompileFragmentShader(out string error)
        {
            IntPtr result = Plugin.CompilePixelShaderFromString(_instanceId, Marshal.StringToHGlobalAnsi($"struct VsOutput {{ float4 pos : SV_POSITION; float2 uv : TEXCOORD0; }}; {_fragmentShaderCode}"));
            string resultString = Marshal.PtrToStringAnsi(result);
            if (!string.IsNullOrEmpty(resultString))
            {
                error = resultString;
                return false;
            }

            error = null;
            return true;
        }
        
        public bool CompileFragmentShaderFromString(string shaderCode, out string error)
        {
            _fragmentShaderCode = shaderCode;
            return CompileFragmentShader(out error);
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
                Plugin.SetConstantBuffer(_instanceId, _constantBufferPtr, Marshal.SizeOf<T>());
            }
            Marshal.StructureToPtr(buffer, _constantBufferPtr, _constantBufferSize > 0);
        }

        public void BlitNow()
        {
            Plugin.Render(_instanceId);
        }
        
        private IEnumerator OnRender()
        {
            while (!_isDestroyed)
            {
                yield return new WaitForEndOfFrame();
                
                if (!isActiveAndEnabled || !RenderEveryFrame || _targetTexture == null) continue;
                GL.IssuePluginEvent(Plugin.GetRenderEventFunc(), _instanceId);
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
            
            Plugin.SetTexturePtr(_instanceId, _targetTexture.GetNativeTexturePtr(), _targetTexture.width, _targetTexture.height, (int)_targetTexture.format.GetDxgiFormat());
        }
        
    }
}