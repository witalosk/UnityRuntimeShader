using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace RuntimeFragmentShader.Sample
{
    public class ShaderEditorView : MonoBehaviour
    {
        [SerializeField] private bool _compileOnCodeChanged = true;
        [Space]
        [SerializeField] private NativeShaderExecutorBase _executor;
        [SerializeField] private TMP_InputField _shaderCodeText;
        [SerializeField] private TextMeshProUGUI _highlightedText;
        [SerializeField] private TextMeshProUGUI _errorText;
        [SerializeField] private Button _compileButton;
        
        private int _prevCompileFrame = -1;
        
        private void Start()
        {
            _compileButton.onClick.AddListener(OnCompileButtonClicked);
            _shaderCodeText.text = _executor.ShaderCode;
            _shaderCodeText.onValueChanged.AddListener(OnShaderCodeChanged);
            OnShaderCodeChanged(_shaderCodeText.text);
        }
        
        public void OnCompileButtonClicked()
        {
            if (_prevCompileFrame == Time.frameCount) return;
            
            if (!_executor.CompileShader(out string error))
            {
                _errorText.text = error;
            }
            else
            {
                _errorText.text = string.Empty;
            }
            
            _prevCompileFrame = Time.frameCount;
        }

        private void OnShaderCodeChanged(string text)
        {
            _executor.ShaderCode = text;
            _highlightedText.text = HlslHighliter.Highlight(text);

            if (_compileOnCodeChanged)
            {
                OnCompileButtonClicked();
            }
        }
    }
}