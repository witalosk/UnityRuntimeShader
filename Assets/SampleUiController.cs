using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace RuntimeFragmentShader.Sample
{
    public class SampleUiController : MonoBehaviour
    {
        [SerializeField]
        private SampleRenderer _sampleRenderer;
        
        [SerializeField]
        private TMP_InputField _shaderCodeText;
        
        [SerializeField]
        private Button _compileButton;

        private void Start()
        {
            _shaderCodeText.text = _sampleRenderer.FragmentShaderCode;
            _compileButton.onClick.AddListener(OnCompileButtonClicked);
        }
        
        public void OnCompileButtonClicked()
        {
            _sampleRenderer.FragmentShaderCode = _shaderCodeText.text;
            _sampleRenderer.CompilePixelShader();
        }
    }
}