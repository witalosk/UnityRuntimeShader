using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace RuntimeFragmentShader.Sample
{
    public class SampleUiController : MonoBehaviour
    {
        [SerializeField] private SampleRenderer _sampleRenderer;
        [SerializeField] private TMP_InputField _shaderCodeText;
        [SerializeField] private Button _compileButton;
        
        private void Start()
        {
            _compileButton.onClick.AddListener(OnCompileButtonClicked);
            _shaderCodeText.text = _sampleRenderer.FragmentShaderCode;
        }
        
        public void OnCompileButtonClicked()
        {
            _sampleRenderer.CompileFragmentShader(_shaderCodeText.text);
        }
    }
}