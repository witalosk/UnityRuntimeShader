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
        public string FragmentShaderCode { get => _fragmentShaderCode; set => _fragmentShaderCode = value; }
        
        [SerializeField]
        private Vector2Int _textureSize = new Vector2Int(256, 256);
        
        [SerializeField, TextArea(10, 20)]
        private string _fragmentShaderCode = @"float4 Frag(VsOutput input) : SV_TARGET
{
	return float4(input.uv, 0.0, 1.0);
}";
        
        [SerializeField]
        private bool _startCompile = false;
        
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
            _shaderRenderer.CompilePixelShaderFromString(_fragmentShaderCode);

            GetComponent<Renderer>().material.mainTexture = _targetTexture;
        }

        private void Update()
        {
            if (_startCompile)
            {
                _shaderRenderer.CompilePixelShaderFromString(_fragmentShaderCode);
                _startCompile = false;
            }
            
            _constantBuffer.Time = Time.time;
            _shaderRenderer.SetConstantBuffer(_constantBuffer);
        }
        
        private void OnDestroy()
        {
            Destroy(_targetTexture);
        }
        
        public void CompilePixelShader()
        {
            _shaderRenderer.CompilePixelShaderFromString(_fragmentShaderCode);
        }
    }
}