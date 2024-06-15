using UnityEngine;
using UnityEditor;
using System;

namespace RuntimeFragmentShader.Editor
{
    /// <summary>
    /// https://tips.hecomi.com/entry/2016/10/13/205422
    /// </summary>
    public class CodeEditor
    {
        public Color BackgroundColor { get; set; }
        public Color TextColor { get; set; }
        public Func<string, string> Highlighter { get; set; }

        private string CashedCode { get; set; }
        private string CachedHighlightedCode { get; set; }

        public CodeEditor()
        {
            BackgroundColor = new Color(0.9f, 0.9f, 0.92f);
            TextColor = new Color(0.6666666667f, 0.6941176471f, 0.7372549020f);
            Highlighter = code => code;
        }

        public string Draw(string code, GUIStyle style, params GUILayoutOption[] options)
        {
            var preBackgroundColor = GUI.backgroundColor;
            var preColor = GUI.color;

            var backStyle = new GUIStyle(style);
            backStyle.normal.textColor = Color.clear;
            backStyle.hover.textColor = Color.clear;
            backStyle.active.textColor = Color.clear;
            backStyle.focused.textColor = Color.clear;

            GUI.backgroundColor = BackgroundColor;

            code = EditorGUILayout.TextArea(code, backStyle, options);
            
            if (string.IsNullOrEmpty(CachedHighlightedCode) || (code != CashedCode))
            {
                CashedCode = code;
                CachedHighlightedCode = Highlighter(code);
            }

            GUI.backgroundColor = Color.clear;

            var foreStyle = new GUIStyle(style);
            foreStyle.normal.textColor = TextColor;
            foreStyle.hover.textColor = TextColor;
            foreStyle.active.textColor = TextColor;
            foreStyle.focused.textColor = TextColor;

            foreStyle.richText = true;

            EditorGUI.TextArea(GUILayoutUtility.GetLastRect(), CachedHighlightedCode, foreStyle);

            GUI.backgroundColor = preBackgroundColor;
            GUI.color = preColor;

            return code;
        }
    }
}