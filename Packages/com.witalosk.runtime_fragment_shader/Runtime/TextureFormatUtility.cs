using System;
using UnityEngine;

namespace RuntimeFragmentShader
{
    public enum DxgiFormat
    {
        DXGI_FORMAT_UNKNOWN = 0,
        DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
        DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
        DXGI_FORMAT_R32G32_FLOAT = 16,
        DXGI_FORMAT_R8G8B8A8_UNORM = 28,
        DXGI_FORMAT_R16G16_FLOAT = 34,
        DXGI_FORMAT_R32_FLOAT = 41,
        DXGI_FORMAT_R16_FLOAT = 54,
        DXGI_FORMAT_B8G8R8A8_UNORM = 87,
    }
    
    public static class TextureFormatUtility
    {

        public static DxgiFormat GetDxgiFormat(this RenderTextureFormat textureFormat)
        {
            switch (textureFormat)
            {
                case RenderTextureFormat.ARGB32:
                    return DxgiFormat.DXGI_FORMAT_R8G8B8A8_UNORM;
                case RenderTextureFormat.BGRA32:
                    return DxgiFormat.DXGI_FORMAT_B8G8R8A8_UNORM;
                case RenderTextureFormat.ARGBFloat:
                    return DxgiFormat.DXGI_FORMAT_R32G32B32A32_FLOAT;
                case RenderTextureFormat.ARGBHalf:
                    return DxgiFormat.DXGI_FORMAT_R16G16B16A16_FLOAT;
                case RenderTextureFormat.RGFloat:
                    return DxgiFormat.DXGI_FORMAT_R32G32_FLOAT;
                case RenderTextureFormat.RGHalf:
                    return DxgiFormat.DXGI_FORMAT_R16G16_FLOAT;
                case RenderTextureFormat.RFloat:
                    return DxgiFormat.DXGI_FORMAT_R32_FLOAT;
                case RenderTextureFormat.RHalf:
                    return DxgiFormat.DXGI_FORMAT_R16_FLOAT;
                default:
                    throw new NotImplementedException();
            }
        }
        
        public static DxgiFormat GetDxgiFormat(this TextureFormat textureFormat)
        {
            switch (textureFormat)
            {
                case TextureFormat.RGBA32:
                    return DxgiFormat.DXGI_FORMAT_R8G8B8A8_UNORM;
                case TextureFormat.BGRA32:
                    return DxgiFormat.DXGI_FORMAT_B8G8R8A8_UNORM;
                case TextureFormat.RGBAFloat:
                    return DxgiFormat.DXGI_FORMAT_R32G32B32A32_FLOAT;
                case TextureFormat.RGBAHalf:
                    return DxgiFormat.DXGI_FORMAT_R16G16B16A16_FLOAT;
                case TextureFormat.RGFloat:
                    return DxgiFormat.DXGI_FORMAT_R32G32_FLOAT;
                case TextureFormat.RGHalf:
                    return DxgiFormat.DXGI_FORMAT_R16G16_FLOAT;
                case TextureFormat.RFloat:
                    return DxgiFormat.DXGI_FORMAT_R32_FLOAT;
                case TextureFormat.RHalf:
                    return DxgiFormat.DXGI_FORMAT_R16_FLOAT;
                default:
                    throw new NotImplementedException();
            }
        }
        
    }
}