using System;
using UnityEngine;

namespace RuntimeFragmentShader.Sample
{
    [RequireComponent(typeof(ShaderRenderer))]
    public class SampleRenderer : MonoBehaviour
    {
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
            _targetTexture = new RenderTexture(256, 256, 0, RenderTextureFormat.ARGBFloat);
            
            _shaderRenderer.TargetTexture = _targetTexture;

            GetComponent<Renderer>().material.mainTexture = _targetTexture;
        }

        private void OnDestroy()
        {
            Destroy(_targetTexture);
        }
    }
}