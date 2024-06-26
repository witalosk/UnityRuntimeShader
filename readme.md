# Unity Runtime Shader
Compile and execute fragment / compute shader at runtime in Unity.

![sample_compute](https://github.com/witalosk/UnityRuntimeShader/assets/23546865/0769f8ad-a885-482f-b38a-642593e9222e)

This package allows the Unity runtime to compile HLSL code and write the results to a RenderTexture or GraphicsBuffer.
This would be useful for VJ events or when you want to adjust post effects at work.

It may not work in some environments, so please contact me if this is the case.

## Environment
- **Windows only**
- **DirectX 11 only**
- Unity 2021.3.0f1 or later

## Installation
1. Open the Unity Package Manager
2. Click the + button
3. Select "Add package from git URL..."
4. Enter `https://github.com/witalosk/UnityRuntimeShader.git?path=Packages/com.witalosk.unity_runtime_shader`

Note: If you want to use the sample scenes, you have to clone this repository and open the project.

## Usage
### Fragment Shader
![sample_fragment](https://github.com/witalosk/UnityRuntimeShader/assets/23546865/a4e54575-5563-43cd-b8e0-4df9dd4562b3)
1. Add `ShaderRenderer` component to your GameObject.
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
        _shaderRenderer.SetConstantBuffer(0, _constantBuffer);
        _shaderRenderer.SetTexture(0, _attachTexture);
        _shaderRenderer.SetBuffer(0, _graphicsBuffer);
    }
```

For details, please refer to the FragmentShaderSample scene.

### Compute Shader
![sample_compute](https://github.com/witalosk/UnityRuntimeShader/assets/23546865/0769f8ad-a885-482f-b38a-642593e9222e)
1. Add `KernelDispatcher` component to your GameObject.
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
        _kernelDispatcher.SetConstantBuffer(0, _constantBuffer);
        _kernelDispatcher.SetBuffer(0, _readBuffer);
        _kernelDispatcher.SetRwBuffer(0, _writeBuffer);
        _kernelDispatcher.SetTexture(1, _texture1);
        _kernelDispatcher.Dispatch(Mathf.CeilToInt(buffer.Read.count / 256f), 1, 1);
    }
```

For details, please refer to the ComputeShaderSample scene.
