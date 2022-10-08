using K4os.Compression.LZ4;
using K4os.Hash.xxHash;

using ShaderTranslator;

if (args.Length > 0)
{
    string translated = Translator.Translate(File.ReadAllBytes(args[0]), out bool isPixelShader);
    File.WriteAllText(args[0] + ".hlsl", translated);
    File.WriteAllBytes(args[0] + ".dxbc", Translator.Compile(translated, isPixelShader));
    return;
}

var cacheInfos = new Dictionary<string, string[]>
{
    {
        "blueblur",
        new[]
        {
            @"D:\Steam\steamapps\common\Sonic Generations\disk\bb3\shader_r.ar.00",
            @"D:\Steam\steamapps\common\Sonic Generations\disk\bb3\shader_r_add.ar.00",
            @"D:\Steam\steamapps\common\Sonic Generations\disk\bb3\shader_s.ar.00",
            @"D:\Steam\steamapps\common\Sonic Generations\disk\bb3\shader_s_add.ar.00",
            @"D:\Steam\steamapps\common\Sonic Generations\disk\bb3\shader_debug.ar.00",
            @"D:\Steam\steamapps\common\Sonic Generations\disk\bb3\shader_debug_add.ar.00",
            @"D:\Steam\steamapps\common\Sonic Generations\mods\Better FxPipeline\disk\bb3\FxFXAA.ar.00",
            @"D:\Steam\steamapps\common\Sonic Generations\mods\Better FxPipeline\disk\bb3\SWA.ar.00"
        }
    },
    #if false
    {
        "pbr",
        new []
        {
            @"D:\Steam\steamapps\common\Sonic Generations\mods\PBR Shaders\disk\bb3\shader_pbr.ar.00",
            @"D:\Steam\steamapps\common\Sonic Generations\mods\PBR Shaders\disk\bb3\shader_vanilla.ar.00"
        }
    }
    #endif
};

foreach (var cacheInfo in cacheInfos)
{
    var shaders = new List<(string Name, byte[] Data, int Position, int Length)>();

    foreach (var archiveFilePath in cacheInfo.Value)
    {
        var archive = new ArchiveDatabase(archiveFilePath);

        foreach (var file in archive.Contents.Where(x => x.Name.EndsWith(".wpu") || x.Name.EndsWith(".wvu")))
        {
            unsafe
            {
                fixed (byte* data = file.Data)
                {
                    // Detect PBR container
                    if (*(uint*)data != 0xFFFF0300 && *(uint*)data != 0xFFFE0300)
                    {
                        byte boolCount = *data;
                        int metadataSize = 1 + boolCount + (1 << boolCount);
                        metadataSize = (metadataSize + 3) & ~3;

                        for (int position = metadataSize; position < file.Data.Length;)
                        {
                            int len = *(int*)(data + position);
                            shaders.Add((file.Name, file.Data, 4 + position, len));
                            position += 4 + len;
                        }
                    }

                    else
                        shaders.Add((file.Name, file.Data, 0, file.Data.Length));
                }
            }
        }
    }

    var shaderMap = new Dictionary<uint, (byte[] Original, byte[] Translated)>();
    var lockObject = new object();

    Parallel.ForEach(shaders, shader =>
    {
        var hash = XXH32.DigestOf(shader.Data, shader.Position, shader.Length);
        lock (lockObject)
        {
            if (shaderMap.TryGetValue(hash, out var pair))
            {
                if (!pair.Original.SequenceEqual(shader.Data))
                    throw new Exception("Hash collision detected!");

                return;
            }
        }

        Console.WriteLine(shader.Name);

        byte[] translated;
        unsafe
        {
            fixed (byte* data = shader.Data)
                translated = Translator.Translate(data + shader.Position, shader.Length);
        }

        lock (lockObject)
            shaderMap[hash] = (shader.Data, translated);
    });

    byte[] bytes;

    using (var stream = new MemoryStream())
    using (var writer = new BinaryWriter(stream))
    {
        foreach (var pair in shaderMap.OrderBy(x => x.Value.Translated.Length))
        {
            writer.Write(pair.Key);
            writer.Write(pair.Value.Translated.Length);
            writer.Write(pair.Value.Translated);

            int padding = ((pair.Value.Translated.Length + 3) & ~3) - pair.Value.Translated.Length;
            for (int i = 0; i < padding; i++)
                writer.Write((byte)0);
        }

        bytes = stream.ToArray();
    }

    var dst = new byte[LZ4Codec.MaximumOutputSize(bytes.Length)];
    var src = bytes;

    int length = LZ4Codec.Encode(src.AsSpan(), dst.AsSpan(), LZ4Level.L12_MAX);

    using (var stream = File.Create($@"D:\Steam\steamapps\common\Sonic Generations\mods\Direct3D 11\{cacheInfo.Key}.shadercache"))
    using (var writer = new BinaryWriter(stream))
    {
        writer.Write(Translator.Version);
        writer.Write(length);
        writer.Write(bytes.Length);
        writer.Write(dst, 0, length);

        int padding = ((length + 3) & ~3) - length;
        for (int i = 0; i < padding; i++)
            writer.Write((byte)0);
    }
}