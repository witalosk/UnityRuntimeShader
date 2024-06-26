using UnityEngine;

namespace UnityRuntimeShader.Sample
{
    public abstract class ModuleBase : MonoBehaviour
    {
        public bool IsActive => _isActive;
        
        [SerializeField] protected bool _isActive = true;
        
        /// <summary>
        /// Dispatch the compute shader.
        /// </summary>
        public abstract void Execute(SwapBuffer buffer);
    }
}