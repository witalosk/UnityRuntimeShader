using UnityEditor;
using UnityEngine;

namespace UnityRuntimeShader.Editor
{
    [CustomEditor(typeof(KernelDispatcher))]
    public class KernelDispatcherDrawer : UnityEditor.Editor
    {
        KernelDispatcher _kernelDispatcher = null;
        CodeEditor _codeEditor;
        Vector2 _scrollPos;
        
        private void OnEnable()
        {
            _kernelDispatcher = target as KernelDispatcher;
            _codeEditor = new CodeEditor
            {
                Highlighter = HlslHighlighter.Highlight
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
            Undo.RecordObject(_kernelDispatcher, "Edit Shader Code");

            EditorGUI.BeginChangeCheck();
            _kernelDispatcher.ShaderCode = _codeEditor.Draw(_kernelDispatcher.ShaderCode, style, GUILayout.ExpandHeight(true));
            EditorGUILayout.EndScrollView();
            if (EditorGUI.EndChangeCheck())
            {
                EditorUtility.SetDirty(_kernelDispatcher);
            }

            if (GUILayout.Button("Compile", GUILayout.Height(24f)))
            {
                if (!_kernelDispatcher.CompileShader(out var error))
                {
                    Debug.LogError(error);
                }
            }
        }
    }
}