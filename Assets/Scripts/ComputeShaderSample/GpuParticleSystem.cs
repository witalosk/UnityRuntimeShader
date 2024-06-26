using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace UnityRuntimeShader.Sample
{
    public class GpuParticleSystem : MonoBehaviour
    {
        [SerializeField] private int _particleCount = 10000;
        [SerializeField] private Color _initialColor = Color.cyan;
        [SerializeField] private float _initialSize = 0.01f;
        [SerializeField] private List<ModuleBase> _modules = new();
        
        private SwapBuffer _particleBuffer;
        
        private void Start()
        {
            _particleBuffer = new SwapBuffer(_particleCount, Marshal.SizeOf<Particle>());
            
            // Set initial particle data
            var cpuBuffer = new Particle[_particleCount];
            for (int i = 0; i < _particleCount; i++)
            {
                var particle = new Particle
                {
                    Uuid = i,
                    Size = _initialSize,
                    Position = Random.insideUnitSphere * 5f,
                    Velocity = Vector3.zero,
                    Color = _initialColor
                };
                cpuBuffer[i] = particle;
            }
            _particleBuffer.Read.SetData(cpuBuffer);
        }

        private void Update()
        {
            foreach (var module in _modules)
            {
                if (!module.IsActive) continue;
                module.Execute(_particleBuffer);
            }
        }

        private void OnDestroy()
        {
            _particleBuffer.Dispose();
        }
    }
}