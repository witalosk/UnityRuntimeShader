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
        public static extern void SetTexturePtr(IntPtr ptr, IntPtr texture, int width, int height, int format);
        
        [DllImport("RuntimeFragmentShader")]
        public static extern void SetConstantBuffer(IntPtr ptr, IntPtr buffer, int size);

        [DllImport("RuntimeFragmentShader")]
        public static extern IntPtr GetRenderEventFunc();
        
        [DllImport("RuntimeFragmentShader")]
        public static extern IntPtr CompilePixelShaderFromString(IntPtr ptr, IntPtr shaderCode);
    }
}