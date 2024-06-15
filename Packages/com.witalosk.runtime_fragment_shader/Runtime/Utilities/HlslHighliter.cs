using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace RuntimeFragmentShader
{
    /// <summary>
    /// https://tips.hecomi.com/entry/2016/10/13/205422
    /// </summary>
    public class HlslHighliter
    {
        private static Regex _regex;
        private static MatchEvaluator _evaluator;

        [UnityEditor.InitializeOnLoadMethod]
        private static void Init()
        {
            string preprocessor = @"(#include|#define|#if|#ifdef|#ifndef|#else|#elif|#endif|#undef)";
            string str = @"(\"".*\"")";
            string structure = @"(cbuffer|struct|int4|int3|int2|uint4|uint3|uint2|float2x2|float3x3|float4x4|float4|float3|float2|Texture2D|Texture3D|RWStructuredBuffer|StructuredBuffer|SamplerState)";
            string type = @"(void|int|uint|float)";
            string symbol = @"[=+\-*/|%]+";
            string variable = @"[a-zA-Z_][a-zA-Z0-9_]*";
            string function = @"[a-zA-Z_][a-zA-Z0-9_]*\s*(?=\()";
            string statement = @"(if|else|for|while|return|break|continue|register)";
            string digit = @"(?<![a-zA-Z_])[+-]?[0-9]+\.?[0-9]?(([eE][+-]?)?[0-9]+)?";
            string comment = @"/\*[\s\S]*?\*/|//.*";

            string block = "(?<{0}>({1}))";
            string pattern = "(" + string.Join("|", new[]
            {
                string.Format(block, "comment", comment),
                string.Format(block, "str", str),
                string.Format(block, "preprocessor", preprocessor),
                string.Format(block, "structure", structure),
                string.Format(block, "type", type),
                string.Format(block, "statement", statement),
                string.Format(block, "symbol", symbol),
                string.Format(block, "function", function),
                string.Format(block, "digit", digit),
                string.Format(block, "variable", variable),
            }) + ")";

            _regex = new Regex(pattern, RegexOptions.Compiled);

            var colorTable = new Dictionary<string, string>()
            {
                { "preprocessor", "#c678dd" },
                { "str", "#90b061" },
                { "structure", "#e5c07b" },
                { "type", "#c678dd" },
                { "symbol", "#61afef" },
                { "variable", "#e06c75" },
                { "function", "#61afef" },
                { "statement", "#c678dd"},
                { "digit", "#be8a59" },
                { "comment", "#90b061" },
            };

            _evaluator = match =>
            {
                foreach (var pair in colorTable)
                {
                    if (match.Groups[pair.Key].Success)
                    {
                        return string.Format("<color={1}>{0}</color>", match.Value, pair.Value);
                    }
                }

                return match.Value;
            };
        }

        public static string Highlight(string code)
        {
            return _regex.Replace(code, _evaluator);
        }
    }
}