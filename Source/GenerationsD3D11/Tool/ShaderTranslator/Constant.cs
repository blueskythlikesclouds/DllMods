using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderTranslator
{
    public class Constant
    {
        public ConstantType Type { get; set; }
        public string Name { get; set; }
        public int Register { get; set; }
        public int Size { get; set; }
    }
}
