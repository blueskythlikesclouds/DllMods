using ShaderTranslator;

var archiveFileNames = new[]
{
    "shader_r",
    "shader_r_add",
    /*"shader_s",
    "shader_s_add",
    "shader_debug",
    "shader_debug_add",*/
};

foreach (var archiveFileName in archiveFileNames)
{
    string archiveFilePath =
        Path.Combine(@"D:\Steam\steamapps\common\Sonic Generations\disk\bb3", archiveFileName + ".ar.00");

    var archiveDatabase = new ArchiveDatabase(archiveFilePath);

    Parallel.ForEach(archiveDatabase.Contents.Where(x => x.Name.EndsWith(".wpu") || x.Name.EndsWith(".wvu")), data =>
    {
        Console.WriteLine(data.Name);

        data.Data = Translator.Translate(data.Data);
        data.Time = DateTime.Now;
    });

    archiveDatabase.Save(Path.Combine(@"D:\Steam\steamapps\common\Sonic Generations\mods\Direct3D 11\disk\bb3", archiveFileName + ".ar.00"));
}