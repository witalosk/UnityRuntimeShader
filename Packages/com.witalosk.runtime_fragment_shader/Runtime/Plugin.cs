using System;
using System.Runtime.InteropServices;

namespace RuntimeFragmentShader
{
    public static class Plugin
    {
        [DllImport("RuntimeFragmentShader")]
        public static extern int CreateRenderer();

        [DllImport("RuntimeFragmentShader")]
        public static extern void ReleaseRenderer(int ptr);

        [DllImport("RuntimeFragmentShader")]
        public static extern void SetTexturePtr(int ptr, IntPtr texture, int width, int height, int format);
        
        [DllImport("RuntimeFragmentShader")]
        public static extern void SetConstantBuffer(int ptr, IntPtr buffer, int size);
        
        [DllImport("RuntimeFragmentShader")]
        public static extern void Render(int ptr);

        [DllImport("RuntimeFragmentShader")]
        public static extern IntPtr GetRenderEventFunc();
        
        [DllImport("RuntimeFragmentShader")]
        public static extern IntPtr CompilePixelShaderFromString(int ptr, IntPtr shaderCode);
    }
}