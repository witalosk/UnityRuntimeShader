using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEditor;
using UnityEditorInternal;
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

        private int _cped = 3;
        private void Update()
        {
            if (_cped > 0 && _modules[1].IsActive)
            {
                RenderDoc.BeginCaptureRenderDoc(EditorWindow.focusedWindow);
            }
            foreach (var module in _modules)
            {
                if (!module.IsActive) continue;
                module.Execute(_particleBuffer);
            }
            if (_modules[1].IsActive && _cped > 0)
            {
                RenderDoc.EndCaptureRenderDoc(EditorWindow.focusedWindow);
                _cped--;
            }
        }

        private void OnDestroy()
        {
            _particleBuffer.Dispose();
        }
    }
}