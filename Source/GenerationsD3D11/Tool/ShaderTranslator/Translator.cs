using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace ShaderTranslator
{
    public static class Translator
    {
        private static void PopulateSemantics(Dictionary<string, string> semantics)
        {
            semantics.Add("SV_Position", "svPos");

            semantics.Add("TEXCOORD", "texCoord");
            for (int i = 1; i < 16; i++)
                semantics.Add($"TEXCOORD{i}", $"texCoord{i}");

            semantics.Add("COLOR", "color");
            semantics.Add("COLOR1", "color1");
        }
        
        public static unsafe byte[] Translate(byte[] function)
        {
            fixed (byte* ptr = function)
                return Translate(ptr, function.Length);
        }

        public static unsafe byte[] Translate(void* function, int functionSize)
        {
            string disassembly;
            {
                ID3DBlob blob;
                Compiler.Disassemble(function, functionSize, 0x40, null, out blob);
                disassembly = Marshal.PtrToStringAnsi(blob.GetBufferPointer());
            }

            int i;

            var lines = disassembly.Split(new[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
            for (i = 0; i < lines.Length; i++)
                lines[i] = lines[i].Trim();

            bool isPixelShader = false;

            var inSemantics = new Dictionary<string, string>();
            var outSemantics = new Dictionary<string, string>();
            var samplers = new Dictionary<string, string>();

            var definitions = new Dictionary<int, string>();
            var definitionsInt = new Dictionary<int, string>();

            for (i = 0; i < lines.Length; i++)
            {
                string line = lines[i];

                if (line.StartsWith("//"))
                    continue;

                if (line.StartsWith("vs_"))
                    PopulateSemantics(outSemantics);

                else if (line.StartsWith("ps_"))
                {
                    PopulateSemantics(inSemantics);
                    isPixelShader = true;
                }

                else if (line.StartsWith("defi"))
                {
                    int firstSeparatorIndex = line.IndexOf(',');
                    definitionsInt.Add(int.Parse(line.Substring(6, firstSeparatorIndex - 6)), line.Substring(firstSeparatorIndex + 2));
                }

                else if (line.StartsWith("def"))
                {
                    int firstSeparatorIndex = line.IndexOf(',');
                    definitions.Add(int.Parse(line.Substring(5, firstSeparatorIndex - 5)), line.Substring(firstSeparatorIndex + 2));
                }

                else if (line.StartsWith("dcl"))
                {
                    int underscoreIndex = line.IndexOf('_');
                    int spaceIndex = line.IndexOf(' ');
                    int dotIndex = line.IndexOf('.');

                    int separatorIndex = line.IndexOf('_', underscoreIndex + 1);
                    if (separatorIndex == -1)
                        separatorIndex = spaceIndex;

                    string semantic = underscoreIndex == -1
                        ? "SV_Position"
                        : line.Substring(underscoreIndex + 1, separatorIndex - underscoreIndex - 1).ToUpperInvariant();

                    string token;

                    if (dotIndex == -1)
                        token = line.Substring(spaceIndex + 1);
                    else
                        token = line.Substring(spaceIndex + 1, dotIndex - spaceIndex - 1);

                    switch (semantic)
                    {
                        case "POSITION" when !isPixelShader && token[0] == 'o':
                            semantic = "SV_Position";
                            break;

                        case "CUBE":
                            semantic = "Cube";
                            break;

                        case "VOLUME":
                            semantic = "3D";
                            break;
                    }

                    switch (token[0])
                    {
                        case 'v':
                            inSemantics[semantic] = token;
                            break;

                        case 'o':
                            outSemantics[semantic] = token;
                            break;

                        case 's':
                            samplers.Add(token, semantic);
                            break;
                    }
                }

                else 
                    break;
            }

            var instructions = new List<Instruction>();
            for (; i < lines.Length; i++)
            {
                if (!lines[i].StartsWith("//"))
                    instructions.Add(new Instruction(lines[i]));
            }

            var constants = ConstantParser.Parse(function, functionSize).OrderBy(x => x.Type).ThenBy(x => x.Register).ToList();
            var constantMap = new Dictionary<string, string>();

            var stringBuilder = new StringBuilder();
            stringBuilder.AppendLine("#define FLT_MAX asfloat(0x7f7fffff)\n");
            stringBuilder.AppendLine("cbuffer global : register(b0) {");

            foreach (var constant in constants)
            {
                switch (constant.Type)
                {
                    case ConstantType.Float4:
                        stringBuilder.AppendFormat("\tfloat4 {0}", constant.Name);

                        if (constant.Size > 1)
                        {
                            stringBuilder.AppendFormat("[{0}]", constant.Size);

                            for (int j = 0; j < constant.Size; j++)
                                constantMap.Add($"c{(constant.Register + j)}", $"{constant.Name}[{j}]");
                        }

                        else
                            constantMap.Add($"c{constant.Register}", constant.Name);

                        stringBuilder.AppendFormat(" : packoffset(c{0});\n", constant.Register);
                        break;

                    case ConstantType.Bool:
                        char swizzle = 'x';

                        switch (constant.Register % 4)
                        {
                            case 1: swizzle = 'y'; break;
                            case 2: swizzle = 'z'; break;
                            case 3: swizzle = 'w'; break;
                        }

                        stringBuilder.AppendFormat("\tbool {0} : packoffset(c{1}.{2});\n", constant.Name,
                            (isPixelShader ? 224 : 256) + constant.Register / 4, swizzle);

                        constantMap.Add($"b{constant.Register}", constant.Name);
                        break;
                }
            }

            stringBuilder.AppendLine("}\n");

            if (isPixelShader)
            {
                stringBuilder.AppendLine("cbuffer alpha_test : register(b1) {");
                stringBuilder.AppendLine("\tbool enable_alpha_test;");
                stringBuilder.AppendLine("\tfloat alpha_threshold;");
                stringBuilder.AppendLine("}\n");
            }

            if (samplers.Count > 0)
            {
                foreach (var constant in constants)
                {
                    if (constant.Type != ConstantType.Sampler)
                        continue;

                    for (int j = 0; j < constant.Size; j++)
                    {
                        string token = $"s{(constant.Register + j)}";
                        string name = constant.Name;

                        if (j > 0)
                            name += $"{j}";
                        
                        stringBuilder.AppendFormat("Texture{0}<float4> {1} : register(t{2});\n",
                            samplers[token], name, constant.Register + j);

                        string type = "SamplerState";

                        if (instructions.Any(x => x.OpCode == "texldp" && x.Arguments[2].Token == token))
                            type = "SamplerComparisonState";

                        stringBuilder.AppendFormat("{0} {1}_s : register({2});\n\n", type, name, token);
                        constantMap.Add(token, name);
                    }
                }
            }

            stringBuilder.AppendLine("void main(");

            foreach (var semantic in inSemantics)
            {
                string type = "float";

                if (semantic.Key == "BLENDINDICES")
                    type = "uint";

                stringBuilder.AppendFormat("\tin {0}4 {1} : {2},\n", type, semantic.Value, semantic.Key);
            }

            if (!isPixelShader)
            {
                int count = 0;

                foreach (var semantic in outSemantics)
                {
                    stringBuilder.AppendFormat("\tout float4 {0} : {1}", semantic.Value, semantic.Key);

                    if ((++count) < outSemantics.Count)
                        stringBuilder.AppendFormat(",\n");
                }
            }
            else
            {
                if (disassembly.Contains("oDepth"))
                    stringBuilder.AppendLine("\tout float oDepth : SV_Depth,");

                stringBuilder.AppendLine("\tout float4 oC0 : SV_Target0,");
                stringBuilder.AppendLine("\tout float4 oC1 : SV_Target1,");
                stringBuilder.AppendLine("\tout float4 oC2 : SV_Target2,");
                stringBuilder.AppendLine("\tout float4 oC3 : SV_Target3");
            }

            stringBuilder.AppendLine(")\n{");

            if (definitions.Count > 0)
            {
                stringBuilder.AppendFormat("\tfloat4 C[{0}];\n\n", definitions.Max(x => x.Key) + 1);

                foreach (var definition in definitions)
                    stringBuilder.AppendFormat("\tC[{0}] = float4({1});\n", definition.Key, definition.Value);

                stringBuilder.AppendLine();
            }

            if (definitionsInt.Count > 0)
            {
                foreach (var definition in definitionsInt)
                    stringBuilder.AppendFormat("\tint4 i{0} = int4({1});\n", definition.Key, definition.Value);
            }

            for (int j = 0; j < 32; j++)
                stringBuilder.AppendFormat("\tfloat4 r{0} = float4(0, 0, 0, 0);\n", j);

            if (!isPixelShader)
                stringBuilder.AppendLine("\tuint4 a0 = uint4(0, 0, 0, 0);");

            stringBuilder.AppendLine();

            int indent = 1;

            foreach (var instruction in instructions)
            {
                if (instruction.Arguments != null)
                {
                    foreach (var argument in instruction.Arguments)
                    {
                        if (argument.Token == "vPos")
                            argument.Token = "(vPos - float4(0.5, 0.5, 0.0, 0.0))";

                        else if (constantMap.TryGetValue(argument.Token, out string constantName))
                            argument.Token = constantName;

                        else if (argument.Token[0] == 'c')
                            argument.Token = $"C[{argument.Token.Substring(1)}]";
                    }
                }

                string instrLine = instruction.ToString();

                if (instrLine.Contains('}')) --indent;

                for (int j = 0; j < indent; j++)
                    stringBuilder.Append("\t");

                instrLine = instrLine.Replace("][", " + ");

                stringBuilder.AppendFormat("{0}\n", instrLine);

                if (instrLine.Contains('{')) ++indent;
            }

            if (isPixelShader)
            {
                stringBuilder.AppendLine("\n\tif (enable_alpha_test) {");
                stringBuilder.AppendLine("\t\tclip(oC0.w - alpha_threshold);");
                stringBuilder.AppendLine("\t}");
            }

            stringBuilder.AppendLine("}");

            string translated = stringBuilder.ToString();
            {
                ID3DBlob blob, errorBlob;

                int result = Compiler.Compile(translated, translated.Length, string.Empty, null, null, "main",
                    isPixelShader ? "ps_5_0" : "vs_5_0", 0, 0, out blob, out errorBlob);

                if (result != 0 && errorBlob != null)
                    throw new Exception(Marshal.PtrToStringAnsi(errorBlob.GetBufferPointer()));

                var bytes = new byte[blob.GetBufferSize()];
                Marshal.Copy(blob.GetBufferPointer(), bytes, 0, blob.GetBufferSize());

                return bytes;
            }
        }
    }
}