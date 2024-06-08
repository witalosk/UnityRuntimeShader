using UnityEditor;
using UnityEngine;

namespace RuntimeFragmentShader.Editor
{
    [CustomEditor(typeof(ShaderRenderer))]
    public class ShaderRendererDrawer : UnityEditor.Editor
    {
        ShaderRenderer _shaderRenderer = null;

        private void OnEnable()
        {
            _shaderRenderer = target as ShaderRenderer;
        }

        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();

            if (GUILayout.Button("Compile", EditorStyles.miniButton))
            {
                if (!_shaderRenderer.CompileFragmentShader(out var error))
                {
                    Debug.LogError(error);
                }
            }
        }
    }
}