using System;
using UnityEngine;

namespace RuntimeFragmentShader.Sample
{
    public class RenderModule : ModuleBase
    {
        [SerializeField] Shader _renderModuleShader;
        
        private Material _renderModuleMaterial;

        private void Start()
        {
            _renderModuleMaterial = new Material(_renderModuleShader);
        }

        private void OnDestroy()
        {
            DestroyImmediate(_renderModuleMaterial);
        }

        public override void Execute(SwapBuffer buffer)
        {
            _renderModuleMaterial.SetBuffer("_ParticleBuffer", buffer.Read);
            Graphics.DrawProcedural(_renderModuleMaterial, new Bounds(Vector3.zero, Vector3.one * 10000), MeshTopology.Points, buffer.Read.count);
        }
    }
}