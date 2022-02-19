using K4os.Compression.LZ4;
using K4os.Hash.xxHash;

using ShaderTranslator;

if (args.Length > 0)
{
    File.WriteAllBytes(args[0] + ".dxbc", Translator.Translate(File.ReadAllBytes(args[0])));
    return;
}

var archiveFileNames = new[]
{
    "shader_r",
    "shader_r_add",
    "shader_s",
    "shader_s_add",
    "shader_debug",
    "shader_debug_add",
};

var obj = new object();
var shaders = new Dictionary<ulong, byte[]>();

foreach (var archiveFileName in archiveFileNames)
{
    string archiveFilePath =
        Path.Combine(@"D:\Steam\steamapps\common\Sonic Generations\disk\bb3", archiveFileName + ".ar.00");

    var archiveDatabase = new ArchiveDatabase(archiveFilePath);

    Parallel.ForEach(archiveDatabase.Contents.Where(x => x.Name.EndsWith(".wpu") || x.Name.EndsWith(".wvu")), data =>
    {
        var hash = XXH64.DigestOf(data.Data.AsSpan());

        lock (obj)
        {
            if (shaders.ContainsKey(hash))
                return;
        }

        Console.WriteLine(data.Name);

        var translated = Translator.Translate(data.Data);
        lock (obj)
        {
            if (shaders.ContainsKey(hash))
                return;

            shaders.Add(hash, translated);
        }
    });
}

byte[] bytes;

using (var stream = new MemoryStream())
using (var writer = new BinaryWriter(stream))
{
    foreach (var pair in shaders)
    {
        writer.Write(pair.Key);
        writer.Write(pair.Value.Length);
        writer.Write(0); // padding
        writer.Write(pair.Value);

        int padding = ((pair.Value.Length + 7) & ~7) - pair.Value.Length;
        for (int i = 0; i < padding; i++)
            writer.Write((byte)0);
    }

    bytes = stream.ToArray();
}

var dst = new byte[LZ4Codec.MaximumOutputSize(bytes.Length)];
var src = bytes;

int length = LZ4Codec.Encode(src.AsSpan(), dst.AsSpan(), LZ4Level.L12_MAX);

using (var stream = File.Create(@"D:\Steam\steamapps\common\Sonic Generations\mods\Direct3D 11\global.shadercache"))
using (var writer = new BinaryWriter(stream))
{
    writer.Write(length);
    writer.Write(bytes.Length);
    writer.Write(dst, 0, length);

    int padding = ((length + 3) & ~3) - length;
    for (int i = 0; i < padding; i++)
        writer.Write((byte)0);
}