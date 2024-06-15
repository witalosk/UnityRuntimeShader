using UnityEngine;

namespace RuntimeFragmentShader
{
    public abstract class NativeShaderExecutorBase : MonoBehaviour
    {
        public abstract string ShaderCode { get; set; }

        public abstract bool CompileShader(out string error);

        public abstract bool CompileShaderFromString(string shaderCode, out string error);
    }
}