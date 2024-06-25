using System;
using System.Runtime.InteropServices;

namespace RuntimeFragmentShader
{
    internal static class Plugin
    {
#region Renderer
        [DllImport("RuntimeFragmentShader")]
        public static extern int CreateRenderer();

        [DllImport("RuntimeFragmentShader")]
        public static extern void ReleaseRenderer(int id);

        [DllImport("RuntimeFragmentShader")]
        public static extern void SetOutputTexture(int id, IntPtr texture, int width, int height, int format);
        
        [DllImport("RuntimeFragmentShader")]
        public static extern void Render(int id);

        [DllImport("RuntimeFragmentShader")]
        public static extern IntPtr GetRenderEventFunc();
        
        [DllImport("RuntimeFragmentShader")]
        public static extern IntPtr CompilePixelShaderFromString(int id, IntPtr shaderCode);
        
        [DllImport("RuntimeFragmentShader")]
        public static extern void SetConstantBuffer(int id, int slot, IntPtr buffer, int size);
        
        [DllImport("RuntimeFragmentShader")]
        public static extern void SetBuffer(int id, int slot, IntPtr buffer, int count, int stride);

        [DllImport("RuntimeFragmentShader")]
        public static extern void SetTexture(int id, int slot, IntPtr texture, int format);
#endregion

#region Dispathcer

        [DllImport("RuntimeFragmentShader")]
        public static extern int CreateDispatcher();

        [DllImport("RuntimeFragmentShader")]
        public static extern void ReleaseDispatcher(int id);
        
        [DllImport("RuntimeFragmentShader")]
        public static extern void Dispatch(int id, int x, int y, int z);
        
        [DllImport("RuntimeFragmentShader")]
        public static extern IntPtr CompileComputeShaderFromString(int id, IntPtr shaderCode);
        
        [DllImport("RuntimeFragmentShader")]
        public static extern void SetConstantBufferToCs(int id, int slot, IntPtr buffer, int size);
        
        [DllImport("RuntimeFragmentShader")]
        public static extern void SetBufferToCs(int id, int slot, IntPtr buffer, int count, int stride);

        [DllImport("RuntimeFragmentShader")]
        public static extern void SetRwBufferToCs(int id, int slot, IntPtr buffer, int count, int stride);

        [DllImport("RuntimeFragmentShader")]
        public static extern void SetTextureToCs(int id, int slot, IntPtr texture, int format);
#endregion
    }
}