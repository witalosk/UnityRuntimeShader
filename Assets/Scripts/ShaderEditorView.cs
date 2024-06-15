using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace RuntimeFragmentShader.Sample
{
    public class ShaderEditorView : MonoBehaviour
    {
        [SerializeField] private NativeShaderExecutorBase _executor;
        [SerializeField] private TMP_InputField _shaderCodeText;
        [SerializeField] private TextMeshProUGUI _highlightedText;
        [SerializeField] private Button _compileButton;
        
        private void Start()
        {
            _compileButton.onClick.AddListener(OnCompileButtonClicked);
            _shaderCodeText.text = _executor.ShaderCode;
            _shaderCodeText.onValueChanged.AddListener(OnShaderCodeChanged);
            OnShaderCodeChanged(_shaderCodeText.text);
        }
        
        public void OnCompileButtonClicked()
        {
            if (!_executor.CompileShader(out string error))
            {
                Debug.LogError(error);
            }
        }

        private void OnShaderCodeChanged(string text)
        {
            _executor.ShaderCode = text;
            _highlightedText.text = HlslHighliter.Highlight(text);
        }
    }
}