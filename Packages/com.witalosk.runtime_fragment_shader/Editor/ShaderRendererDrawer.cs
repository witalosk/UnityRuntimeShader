using UnityEditor;
using UnityEngine;

namespace RuntimeFragmentShader.Editor
{
    [CustomEditor(typeof(ShaderRenderer))]
    public class ShaderRendererDrawer : UnityEditor.Editor
    {
        ShaderRenderer _shaderRenderer = null;
        CodeEditor _codeEditor;
        Vector2 _scrollPos;
        
        private void OnEnable()
        {
            _shaderRenderer = target as ShaderRenderer;
            _codeEditor = new CodeEditor
            {
                Highlighter = HlslHighliter.Highlight
            };
        }

        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();
            
            var style = new GUIStyle(GUI.skin.textArea)
            {
                padding = new RectOffset(6, 6, 6, 6),
                fontSize = 12,
                wordWrap = false
            };
            
            _scrollPos = EditorGUILayout.BeginScrollView(_scrollPos, GUILayout.Height(200));
            _shaderRenderer.ShaderCode = _codeEditor.Draw(_shaderRenderer.ShaderCode, style, GUILayout.ExpandHeight(true));
            EditorGUILayout.EndScrollView();

            if (GUILayout.Button("Compile", GUILayout.Height(24f)))
            {
                if (!_shaderRenderer.CompileShader(out var error))
                {
                    Debug.LogError(error);
                }
            }
        }
    }
}