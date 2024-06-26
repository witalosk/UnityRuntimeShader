using System;
using UnityEngine;

namespace UnityRuntimeShader.Sample
{
    public struct SampleFragmentConstantBuffer
    {
        public float Time;
        public Vector2 Size;
    }
    
    [RequireComponent(typeof(ShaderRenderer))]
    public class SampleRenderer : MonoBehaviour
    {
        [SerializeField] private Vector2Int _textureSize = new(256, 256);
        [SerializeField] private Texture2D _attachTexture;
        
        private SampleFragmentConstantBuffer _constantBuffer;
        private RenderTexture _targetTexture;
        private ShaderRenderer _shaderRenderer;

        private void Start()
        {
            _shaderRenderer = GetComponent<ShaderRenderer>();
            if (_shaderRenderer == null)
            {
                _shaderRenderer = gameObject.AddComponent<ShaderRenderer>();
            }
            _targetTexture = new RenderTexture(_textureSize.x, _textureSize.y, 0, RenderTextureFormat.Default);
            _shaderRenderer.TargetTexture = _targetTexture;

            GetComponent<Renderer>().material.mainTexture = _targetTexture;
        }

        private void Update()
        {
            _constantBuffer.Time = Time.time;
            _constantBuffer.Size = new Vector2(transform.lossyScale.x, transform.lossyScale.y);
            _shaderRenderer.SetTexture(0, _attachTexture);
            _shaderRenderer.SetConstantBuffer(0, _constantBuffer);
        }
        
        private void OnDestroy()
        {
            Destroy(_targetTexture);
        }
    }
}