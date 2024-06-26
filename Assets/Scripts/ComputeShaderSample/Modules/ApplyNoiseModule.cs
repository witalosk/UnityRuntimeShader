using UnityEngine;

namespace UnityRuntimeShader.Sample
{
    public class ApplyNoiseModule : ModuleBase
    {
        [SerializeField] private ComputeShader _applyNoiseCs;
        [Space]
        [SerializeField] private float _noiseFrequency = 0.01f;
        [SerializeField] private float _noiseScale = 0.01f;
        [SerializeField] private float _noiseSpeed = 0.01f;
        
        public override void Execute(SwapBuffer buffer)
        {
            int kernelId = _applyNoiseCs.FindKernel("ApplyNoise");
            _applyNoiseCs.SetFloat("_DeltaTime", Time.deltaTime);
            _applyNoiseCs.SetFloat("_SimulationTime", Time.time);
            _applyNoiseCs.SetFloat("_NoiseFrequency", _noiseFrequency);
            _applyNoiseCs.SetFloat("_NoiseScale", _noiseScale);
            _applyNoiseCs.SetFloat("_NoiseSpeed", _noiseSpeed);
            _applyNoiseCs.SetBuffer(0, "_ParticleReadBuffer", buffer.Read);
            _applyNoiseCs.SetBuffer(0, "_ParticleWriteBuffer", buffer.Write);
            _applyNoiseCs.DispatchDesired(kernelId, buffer.Read.count);
            
            buffer.Swap();
        }
    }
}