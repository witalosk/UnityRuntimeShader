using UnityEngine;

namespace UnityRuntimeShader.Sample
{
    public class IntegrateModule : ModuleBase
    {
        [SerializeField] private ComputeShader _integrateCs;
        
        public override void Execute(SwapBuffer buffer)
        {
            int kernelId = _integrateCs.FindKernel("Integrate");
            _integrateCs.SetFloat("_DeltaTime", Time.deltaTime);
            _integrateCs.SetBuffer(kernelId, "_ParticleReadBuffer", buffer.Read);
            _integrateCs.SetBuffer(kernelId, "_ParticleWriteBuffer", buffer.Write);
            _integrateCs.DispatchDesired(kernelId, buffer.Read.count);
                        
            buffer.Swap();
        }
    }
}