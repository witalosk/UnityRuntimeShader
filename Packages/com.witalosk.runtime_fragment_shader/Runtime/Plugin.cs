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
        public static extern void SetOutputTexture(int ptr, IntPtr texture, int width, int height, int format);
        
        [DllImport("RuntimeFragmentShader")]
        public static extern void Render(int ptr);

        [DllImport("RuntimeFragmentShader")]
        public static extern IntPtr GetRenderEventFunc();
        
        [DllImport("RuntimeFragmentShader")]
        public static extern IntPtr CompilePixelShaderFromString(int ptr, IntPtr shaderCode);
        
        [DllImport("RuntimeFragmentShader")]
        public static extern void SetConstantBuffer(int ptr, IntPtr buffer, int size);

        [DllImport("RuntimeFragmentShader")]
        public static extern void SetTexture(int id, int slot, IntPtr texture, int format);
    }
}