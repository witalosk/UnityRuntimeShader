using System.Collections;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using UnityEngine;

namespace RuntimeFragmentShader
{
    /// <summary>
    /// Highlight HLSL code.
    /// cf: https://tips.hecomi.com/entry/2016/10/13/205422
    /// </summary>
    public static class HlslHighlighter
    {
        public enum Element
        {
            Preprocessor,
            Str,
            Structure,
            Type,
            Symbol,
            Variable,
            Function,
            Statement,
            Digit,
            Comment,
        }

        private static readonly Dictionary<Element, string> PatternTable = new()
        {
            { Element.Preprocessor, @"(#include|#define|#if|#ifdef|#ifndef|#else|#elif|#endif|#undef)" },
            { Element.Str, @"(\"".*\"")" },
            { Element.Structure, @"(cbuffer|struct|int4|int3|int2|uint4|uint3|uint2|float2x2|float3x3|float4x4|float4|float3|float2|Texture2D|Texture3D|RWStructuredBuffer|StructuredBuffer|SamplerState)" },
            { Element.Type, @"(void|int|uint|float)" },
            { Element.Symbol, @"[=+\-*/|%]+" },
            { Element.Variable, @"[a-zA-Z_][a-zA-Z0-9_]*" },
            { Element.Function, @"[a-zA-Z_][a-zA-Z0-9_]*\s*(?=\()" },
            { Element.Statement, @"(if|else|for|while|return|break|continue|register)" },
            { Element.Digit, @"(?<![a-zA-Z_])[+-]?[0-9]+\.?[0-9]?(([eE][+-]?)?[0-9]+)?" },
            { Element.Comment, @"/\*[\s\S]*?\*/|//.*" },
        };
        
        private static readonly Dictionary<Element, string> GroupNameTable = new()
        {
            { Element.Preprocessor, "preprocessor" },
            { Element.Str, "str" },
            { Element.Structure, "structure" },
            { Element.Type, "type" },
            { Element.Symbol, "symbol" },
            { Element.Variable, "variable" },
            { Element.Function, "function" },
            { Element.Statement, "statement" },
            { Element.Digit, "digit" },
            { Element.Comment, "comment" },
        };
        
        private static HighlighterColorTable _colorTable = new();

        private static bool _initialized = false;
        private static Regex _regex;
        private static MatchEvaluator _evaluator;

        /// <summary>
        /// Init the highlighter with the color table.
        /// </summary>
        private static void Init(HighlighterColorTable colorTable = null)
        {
            if (colorTable != null) _colorTable = colorTable;
            
            string block = "(?<{0}>({1}))";
            string pattern = "(" + string.Join("|", new[]
            {
                string.Format(block, GroupNameTable[Element.Comment], PatternTable[Element.Comment]),
                string.Format(block, GroupNameTable[Element.Str], PatternTable[Element.Str]),
                string.Format(block, GroupNameTable[Element.Preprocessor], PatternTable[Element.Preprocessor]),
                string.Format(block, GroupNameTable[Element.Structure], PatternTable[Element.Structure]),
                string.Format(block, GroupNameTable[Element.Type], PatternTable[Element.Type]),
                string.Format(block, GroupNameTable[Element.Statement], PatternTable[Element.Statement]),
                string.Format(block, GroupNameTable[Element.Symbol], PatternTable[Element.Symbol]),
                string.Format(block, GroupNameTable[Element.Function], PatternTable[Element.Function]),
                string.Format(block, GroupNameTable[Element.Digit], PatternTable[Element.Digit]),
                string.Format(block, GroupNameTable[Element.Variable], PatternTable[Element.Variable]),
            }) + ")";

            _regex = new Regex(pattern, RegexOptions.Compiled);

            _evaluator = match =>
            {
                foreach (var pair in _colorTable)
                {
                    if (match.Groups[GroupNameTable[pair.Key]].Success)
                    {
                        return string.Format("<color={1}>{0}</color>", match.Value, pair.Value);
                    }
                }

                return match.Value;
            };

            _initialized = true;
        }

        /// <summary>
        /// Highlight the HLSL code.
        /// </summary>
        public static string Highlight(string code)
        {
            if (!_initialized) Init();
            return _regex.Replace(code, _evaluator);
        }
    }

    /// <summary>
    /// Color table for the highlighter.
    /// </summary>
    public class HighlighterColorTable : IEnumerable<KeyValuePair<HlslHighlighter.Element, string>>
    {
        public string this[HlslHighlighter.Element key]
        {
            get => _colorTable[key];
            set => _colorTable[key] = value;
        }

        private readonly Dictionary<HlslHighlighter.Element, string> _colorTable = new()
        {
            { HlslHighlighter.Element.Preprocessor, "#c678dd" },
            { HlslHighlighter.Element.Str, "#90b061" },
            { HlslHighlighter.Element.Structure, "#e5c07b" },
            { HlslHighlighter.Element.Type, "#c678dd" },
            { HlslHighlighter.Element.Symbol, "#61afef" },
            { HlslHighlighter.Element.Variable, "#e06c75" },
            { HlslHighlighter.Element.Function, "#61afef" },
            { HlslHighlighter.Element.Statement, "#c678dd" },
            { HlslHighlighter.Element.Digit, "#be8a59" },
            { HlslHighlighter.Element.Comment, "#90b061" },
        };
        
        public HighlighterColorTable() { }
        
        public HighlighterColorTable(string preprocessor, string str, string structure, string type, string symbol, string variable, string function, string statement, string digit, string comment)
        {
            _colorTable[HlslHighlighter.Element.Preprocessor] = preprocessor;
            _colorTable[HlslHighlighter.Element.Str] = str;
            _colorTable[HlslHighlighter.Element.Structure] = structure;
            _colorTable[HlslHighlighter.Element.Type] = type;
            _colorTable[HlslHighlighter.Element.Symbol] = symbol;
            _colorTable[HlslHighlighter.Element.Variable] = variable;
            _colorTable[HlslHighlighter.Element.Function] = function;
            _colorTable[HlslHighlighter.Element.Statement] = statement;
            _colorTable[HlslHighlighter.Element.Digit] = digit;
            _colorTable[HlslHighlighter.Element.Comment] = comment;
        }
        
        public HighlighterColorTable(Color preprocessor, Color str, Color structure, Color type, Color symbol, Color variable, Color function, Color statement, Color digit, Color comment)
        {
            SetColor(HlslHighlighter.Element.Preprocessor, preprocessor);
            SetColor(HlslHighlighter.Element.Str, str);
            SetColor(HlslHighlighter.Element.Structure, structure);
            SetColor(HlslHighlighter.Element.Type, type);
            SetColor(HlslHighlighter.Element.Symbol, symbol);
            SetColor(HlslHighlighter.Element.Variable, variable);
            SetColor(HlslHighlighter.Element.Function, function);
            SetColor(HlslHighlighter.Element.Statement, statement);
            SetColor(HlslHighlighter.Element.Digit, digit);
            SetColor(HlslHighlighter.Element.Comment, comment);
        }
        
        public void SetColor(HlslHighlighter.Element key, Color col)
        {
            _colorTable[key] = $"#{ColorUtility.ToHtmlStringRGB(col)}";
        }

        public IEnumerator<KeyValuePair<HlslHighlighter.Element, string>> GetEnumerator()
        {
            return _colorTable.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }
}