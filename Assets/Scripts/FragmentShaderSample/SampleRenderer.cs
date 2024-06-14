using System;
using UnityEngine;

namespace RuntimeFragmentShader.Sample
{
    public struct SampleConstantBuffer
    {
        public float Time;
    }
    
    [RequireComponent(typeof(ShaderRenderer))]
    public class SampleRenderer : MonoBehaviour
    {
        public string FragmentShaderCode => _shaderRenderer.FragmentShaderCode;
        
        [SerializeField] private Vector2Int _textureSize = new(256, 256);
        [SerializeField] private Texture2D _attachTexture;
        
        private SampleConstantBuffer _constantBuffer;
        private RenderTexture _targetTexture;
        private ShaderRenderer _shaderRenderer;

        private void Start()
        {
            _shaderRenderer = GetComponent<ShaderRenderer>();
            if (_shaderRenderer == null)
            {
                _shaderRenderer = gameObject.AddComponent<ShaderRenderer>();
                return;
            }
            _targetTexture = new RenderTexture(_textureSize.x, _textureSize.y, 0, RenderTextureFormat.ARGBFloat);
            _shaderRenderer.TargetTexture = _targetTexture;

            GetComponent<Renderer>().material.mainTexture = _targetTexture;
        }

        private void Update()
        {
            _constantBuffer.Time = Time.time;
            _shaderRenderer.SetTexture(0, _attachTexture);
            _shaderRenderer.SetConstantBuffer(_constantBuffer);
        }
        
        private void OnDestroy()
        {
            Destroy(_targetTexture);
        }
        
        public void CompileFragmentShader(string fragmentShaderCode)
        {
            if (_shaderRenderer.CompileFragmentShaderFromString(fragmentShaderCode, out string error))
            {
                Debug.LogError(error);
            }
        }
    }
}