using System.IO;
using System.Text.RegularExpressions;
using UnityEngine;

namespace RuntimeFragmentShader
{
    public static class ShaderPrecompileProcessor
    {
        /// <summary>
        /// Process the include directive in the shader code.
        /// </summary>
        public static string ProcessInclude(string shaderCode, string includePath = null)
        {
            if (includePath == null) includePath = Application.streamingAssetsPath;
            var includeMatches = Regex.Matches(shaderCode, @"^(?!//)#include ""([^""]+)""");
            foreach (Match match in includeMatches)
            {
                string includeFileName = match.Groups[1].Value;
                string includeStr = File.ReadAllText($"{includePath}/{includeFileName}");
                shaderCode = shaderCode.Replace($"#include \"{includeFileName}\"", $"\n{includeStr}");
            }
            
            return shaderCode;
        }

        /// <summary>
        /// Get the kernel thread group sizes from the shader code.
        /// </summary>
        public static Vector3Int GetKernelThreadGroupSizes(string shaderCode)
        {
            var match = Regex.Match(shaderCode, @"^(?!//)\[numthreads\((\d+),\s*(\d+),\s*(\d+)\)\]");
            if (match.Success)
            {
                return new Vector3Int(
                    int.Parse(match.Groups[1].Value),
                    int.Parse(match.Groups[2].Value),
                    int.Parse(match.Groups[3].Value)
                );
            }

            return Vector3Int.zero;
        }
    }
}