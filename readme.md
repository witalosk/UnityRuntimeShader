# Unity Runtime Shader
Compile and execute fragment / compute shader at runtime in Unity.

[//]: # (![thumbnail]&#40;./ScreenShots/urs.png&#41;)

This package allows the Unity runtime to compile HLSL code and write the results to a RenderTexture or GraphicsBuffer.
This would be useful for VJ events or when you want to adjust post effects at work.

It may not work in some environments, so please contact us if this is the case.

## Environment
- **Windows only**
- **DirectX 11 only**
- Unity 2021.3.0f1 or later

## Installation
1. Open the Unity Package Manager
2. Click the + button
3. Select "Add package from git URL..."
4. Enter `https://github.com/witalosk/UnityRuntimeShader.git?path=Packages/com.witalosk.unity_runtime_shader`

## Usage
### Fragment Shader
1. Add `Shader Renderer` component to your GameObject.
2. Assign a RenderTexture to the `ShaderRenderer.TargetTexture` field.
3. If you want to pass a texture / buffer to the shader, assign it to the `ShaderRenderer.SetTexture()` / `ShaderRenderer.SetBuffer()` / `ShaderRenderer.SetConstantBuffer()` method.
4. Compile shader with `ShaderRenderer.CompileShaderFromString(string shaderCode, out string error);`

### Note
- You have to define resources with register keyword like `Texture2D _Texture1 : register(t0);` in shader.
- Fragment shader function names are fixed to "Frag".
- StreamingAssets files can be included.
- By default, the rendering is performed every frame, but by setting "RenderEveryFrame" to false and using "ShaderRenderer.BlitNow()", the rendering can be performed at any desired timing.

```c#
    private void Start()
    {
        _shaderRenderer = GetComponent<ShaderRenderer>();
        _targetTexture = new RenderTexture(_textureSize.x, _textureSize.y, 0, RenderTextureFormat.Default);
        _shaderRenderer.TargetTexture = _targetTexture;
    }
    
    private void Update()
    {
        _constantBuffer.Time = Time.time;
        _constantBuffer.Size = new Vector2(transform.lossyScale.x, transform.lossyScale.y);
        _shaderRenderer.SetConstantBuffer(_constantBuffer);
        _shaderRenderer.SetTexture(0, _attachTexture);
        _shaderRenderer.SetBuffer(0, _graphicsBuffer);
    }
```

For details, please refer to the FragmentShaderSample scene.

### Compute Shader
1. Add `Kernel Dispatcher` component to your GameObject.
2. If you want to pass a texture / buffer to the shader, assign it to the `KernelDispatcher.SetTexture()` / `KernelDispatcher.SetBuffer()` / `KernelDispatcher.SetRwBuffer()` / `KernelDispatcher.SetConstantBuffer()` method.
3. Compile shader with `KernelDispatcher.CompileShaderFromString(string shaderCode, out string error);`

### Note
- You have to define resources with register keyword like `RWStructuredBuffer<Particle> _WriteBuffer : register(u0);` in shader.
- Compute shader function names are fixed to "Main".
- StreamingAssets files can be included.
- For now, outputting to render texture is not supported.

```c#
    private void Start()
    {
        _kernelDispatcher = GetComponent<KernelDispatcher>();
    }

    private void Update()
    {
        _constantBuffer.Time = Time.time;
        _constantBuffer.DeltaTime = Time.deltaTime;
        _kernelDispatcher.SetConstantBuffer(_constantBuffer);
        _kernelDispatcher.SetBuffer(0, _readBuffer);
        _kernelDispatcher.SetRwBuffer(0, _writeBuffer);
        _kernelDispatcher.SetTexture(1, _texture1);
        _kernelDispatcher.Dispatch(Mathf.CeilToInt(buffer.Read.count / 256f), 1, 1);
    }
```

For details, please refer to the ComputeShaderSample scene.
