using UnityEngine;

namespace UnityRuntimeShader.Sample
{
    public class FpsSetter : MonoBehaviour
    {
        [SerializeField] private int _fps = 60;
        
        private void Start()
        {
            Application.targetFrameRate = _fps;
            QualitySettings.vSyncCount = 0;
        }
    }
}