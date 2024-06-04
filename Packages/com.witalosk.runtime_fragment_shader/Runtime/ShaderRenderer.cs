using System;
using System.Collections;
using UnityEngine;

namespace RuntimeFragmentShader
{
    public class ShaderRenderer : MonoBehaviour
    {
        public RenderTexture TargetTexture { get => _targetTexture; set => ChangeTargetTexture(value); }
        
        private IntPtr _instancePtr = IntPtr.Zero;
        private RenderTexture _targetTexture;
        private bool _isDestroyed = false;

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
        }
        
        public void CompilePixelShaderFromString(string shaderCode)
        {
            Plugin.CompilePixelShaderFromString(_instancePtr, shaderCode);
        }

        IEnumerator OnRender()
        {
            while (!_isDestroyed)
            {
                yield return new WaitForEndOfFrame();
                
                if (!isActiveAndEnabled || _targetTexture == null) continue;
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
            
            Plugin.SetTexturePtr(_instancePtr, _targetTexture.GetNativeTexturePtr(), (int)_targetTexture.format.GetDxgiFormat());
        }
        
    }
}