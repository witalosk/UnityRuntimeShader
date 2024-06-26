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
        
        private void Start()
        {
            _kernelDispatcher = GetComponent<KernelDispatcher>();
            if (_kernelDispatcher == null)
            {
                _kernelDispatcher = gameObject.AddComponent<KernelDispatcher>();
            }
        }
        
        public override void Execute(SwapBuffer buffer)
        {
            _constantBuffer.Time = Time.time;
            _constantBuffer.DeltaTime = Time.deltaTime;
            _kernelDispatcher.SetConstantBuffer(0, _constantBuffer);
            _kernelDispatcher.SetBuffer(0, buffer.Read);
            _kernelDispatcher.SetRwBuffer(0, buffer.Write);
            _kernelDispatcher.DispatchDesired(buffer.Read.count);
            buffer.Swap();
        }
    }
}