using System;
using UnityEngine;

namespace RuntimeFragmentShader.Sample
{
    public struct SampleComputeConstantBuffer
    {
        public float Time;
        public float DeltaTime;
    }
    
    public class CustomHlslModule : ModuleBase
    {
        private SampleComputeConstantBuffer _constantBuffer;
        private KernelDispatcher _kernelDispatcher;
        
        private GraphicsBuffer _tempBuffer;
        RenderTexture _tempTexture;

        
        private void Start()
        {
            _kernelDispatcher = GetComponent<KernelDispatcher>();
            if (_kernelDispatcher == null)
            {
                _kernelDispatcher = gameObject.AddComponent<KernelDispatcher>();
            }
            
            _tempTexture = new RenderTexture(1024, 1024, 0, RenderTextureFormat.ARGB32);
        }

        private void OnDestroy()
        {
            DestroyImmediate(_tempTexture);
        }

        public override void Execute(SwapBuffer buffer)
        {
            _constantBuffer.Time = Time.time;
            _constantBuffer.DeltaTime = Time.deltaTime;
            _kernelDispatcher.SetConstantBuffer(_constantBuffer);
            _kernelDispatcher.SetTexture(0, _tempTexture);
            _kernelDispatcher.SetRwBuffer(0, buffer.Read);
            _kernelDispatcher.SetRwBuffer(1, buffer.Write);
            _kernelDispatcher.Dispatch(Mathf.CeilToInt(buffer.Read.count / 256f), 1, 1);
            buffer.Swap();
        }
    }
}