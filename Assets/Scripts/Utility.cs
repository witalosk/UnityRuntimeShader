using System;
using UnityEngine;

namespace UnityRuntimeShader.Sample
{
    public static class Utility
    {
        /// <summary>
        /// Dispatch the compute shader with the desired thread num.
        /// </summary>
        public static void DispatchDesired(this ComputeShader cs, int kernel, int desiredX, int desiredY = 1, int desiredZ = 1)
        {
            cs.GetKernelThreadGroupSizes(kernel, out uint x, out uint y, out uint z);
            cs.Dispatch(kernel, Mathf.CeilToInt(desiredX / (float)x), Mathf.CeilToInt(desiredY / (float)y), Mathf.CeilToInt(desiredZ / (float)z));
        }
    }

    public class SwapBuffer : IDisposable
    {
        public GraphicsBuffer Read => _readBuffer;
        public GraphicsBuffer Write => _writeBuffer;
        
        private GraphicsBuffer _readBuffer;
        private GraphicsBuffer _writeBuffer;
        
        public SwapBuffer(int count, int stride)
        {
            _readBuffer = new GraphicsBuffer(GraphicsBuffer.Target.Structured, count, stride);
            _readBuffer.name = "buf1";
            _writeBuffer = new GraphicsBuffer(GraphicsBuffer.Target.Structured, count, stride);
            _writeBuffer.name = "buf2";
        }
        
        ~SwapBuffer()
        {
            Dispose();
        }
        
        public void Swap()
        {
            (_readBuffer, _writeBuffer) = (_writeBuffer, _readBuffer);
        }

        public void Dispose()
        {
            _readBuffer?.Dispose();
            _writeBuffer?.Dispose();
            _readBuffer = null;
            _writeBuffer = null;
        }
    }
}