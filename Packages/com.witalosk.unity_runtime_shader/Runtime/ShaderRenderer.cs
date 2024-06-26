using System;
using System.Collections;
using System.Runtime.InteropServices;
using UnityEngine;

namespace UnityRuntimeShader
{
    public class ShaderRenderer : NativeShaderExecutorBase
    {
        public bool RenderEveryFrame { get => _renderEveryFrame; set => _renderEveryFrame = value; }
        public RenderTexture TargetTexture { get => _targetTexture; set => ChangeTargetTexture(value); }
        public override string ShaderCode {get => _fragmentShaderCode; set => _fragmentShaderCode = value;}
        protected override IntPtr PluginCompileShaderFromString(IntPtr shaderCode) => Plugin.CompilePixelShaderFromString(_instanceId, shaderCode);
        protected override void PluginSetTexture(int slot, IntPtr texture, int format) => Plugin.SetTexture(_instanceId, slot, texture, format);
        protected override void PluginSetBuffer(int slot, IntPtr buffer, int count, int stride) => Plugin.SetBuffer(_instanceId, slot, buffer, count, stride);
        protected override void PluginSetConstantBuffer(int slot, IntPtr buffer, int size) => Plugin.SetConstantBuffer(_instanceId, slot, buffer, size);
        
        [SerializeField] private RenderTexture _targetTexture;
        [SerializeField] private bool _renderEveryFrame = true;
        
        [Space]
        [SerializeField, HideInInspector]
        private string _fragmentShaderCode = @"float4 Frag(VsOutput input) : SV_TARGET
{
    return float4(input.uv, 1.0 - input.uv.x, 1.0);
}";
        
        private bool _isDestroyed = false;

        private void Awake()
        {
            _instanceId = Plugin.CreateRenderer();
            
            if (!CompileShader(out string error))
            {
                Debug.LogError(error);
            }
            
            if (_targetTexture != null)
            {
                ChangeTargetTexture(_targetTexture);
            }

            StartCoroutine(OnRender());
        }

        protected override void OnDestroy()
        {
            _isDestroyed = true;
            Plugin.ReleaseRenderer(_instanceId);
            base.OnDestroy();
        }

        public void BlitNow()
        {
            if (_instanceId < 0) return;
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
            if (_instanceId < 0) return;
            _targetTexture = texture;
            
            if (!_targetTexture.IsCreated())
            {
                _targetTexture.Create();
            }
            
            Plugin.SetOutputTexture(_instanceId, _targetTexture.GetNativeTexturePtr(), _targetTexture.width, _targetTexture.height, (int)_targetTexture.format.GetDxgiFormat());
        }
        
    }
}