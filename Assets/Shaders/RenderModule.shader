Shader "Hidden/RenderModule"
{
    HLSLINCLUDE
    #include "UnityCG.cginc"
    #include "Common.hlsl"

    struct v2g
    {
        float3 position : TEXCOORD0;
        float size : TEXCOORD1;
        float4 color : COLOR;
    };

    struct g2f
    {
        float4 position : POSITION;
        float2 texcoord : TEXCOORD0;
        float4 color : COLOR;
    };

    StructuredBuffer<Particle> _ParticleBuffer;
    
    static const float3 g_positions[4] = {float3(-1, 1, 0), float3(1, 1, 0), float3(-1, -1, 0), float3(1, -1, 0)};
    static const float2 g_texcoords[4] = {float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1)};

    v2g Vert(uint id : SV_VertexID)
    {
        Particle p = _ParticleBuffer[id];
        
        v2g o = (v2g)0;
        o.position = UnityObjectToViewPos(p.position);
        o.size = p.size;
        o.color = p.color;
        
        return o;
    }

    [maxvertexcount(4)]
    void Geom(point v2g In[1], inout TriangleStream<g2f> SpriteStream)
    {
        g2f o = (g2f)0;
        [unroll]
        for (int i = 0; i < 4; i++)
        {
            float3 position = g_positions[i] * In[0].size + In[0].position;
            o.position = mul(UNITY_MATRIX_P, float4(position, 1));

            o.color = In[0].color;
            o.texcoord = g_texcoords[i];

            SpriteStream.Append(o);
        }

        SpriteStream.RestartStrip();
    }

    float4 Frag(g2f i) : SV_Target
    {
        return length(i.texcoord - 0.5) < 0.5 ? i.color : float4(0, 0, 0, 0);
    }
    ENDHLSL

    SubShader
    {
        Tags
        {
            "RenderType" = "Transparent" "IgnoreProjector" = "True" "RenderType" = "Transparent"
        }
        LOD 100

        ZWrite Off
        Blend One One

        Pass
        {
            HLSLPROGRAM
            #pragma target   5.0
            #pragma vertex   Vert
            #pragma geometry Geom
            #pragma fragment Frag
            ENDHLSL
        }
    }
}