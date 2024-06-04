using System;
using System.Runtime.InteropServices;

namespace RuntimeFragmentShader
{
    public static class Plugin
    {
        [DllImport("RuntimeFragmentShader")]
        public static extern IntPtr CreateRenderer();

        [DllImport("RuntimeFragmentShader")]
        public static extern void ReleaseRenderer(IntPtr ptr);

        [DllImport("RuntimeFragmentShader")]
        public static extern void SetTexturePtr(IntPtr ptr, IntPtr texture, int format);

        [DllImport("RuntimeFragmentShader")]
        public static extern IntPtr GetRenderEventFunc();
        
        [DllImport("RuntimeFragmentShader")]
        public static extern void CompilePixelShaderFromString(IntPtr ptr, IntPtr shaderCode);
    }
}