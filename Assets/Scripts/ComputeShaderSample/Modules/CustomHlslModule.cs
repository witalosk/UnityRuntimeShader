using UnityEngine;

namespace RuntimeFragmentShader.Sample
{
    public class CustomHlslModule : ModuleBase
    {
        public override void Execute(SwapBuffer buffer)
        {
            
            
            buffer.Swap();
        }
    }
}