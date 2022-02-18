using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace ShaderTranslator
{
    public enum ConflictPolicy
    {
        RaiseError,
        Replace,
        Ignore
    }

    public class DatabaseData
    {
        public string Name { get; set; }
        public byte[] Data { get; set; }
        public DateTime Time { get; set; }

        public DatabaseData()
        {
            Time = DateTime.Now;
        }
    }

    public class ArchiveDatabase
    {
        public List<DatabaseData> Contents { get; }

        public void LoadSingle(string filePath)
        {
            var time = File.GetLastWriteTime(filePath);
            var stringBuilder = new StringBuilder();

            using (var stream = File.OpenRead(filePath))
            using (var reader = new BinaryReader(stream, Encoding.UTF8))
            {
                stream.Seek(16, SeekOrigin.Begin);
                while (stream.Position < stream.Length)
                {
                    long currentOffset = stream.Position;
                    long blockSize = reader.ReadUInt32();
                    long dataSize = reader.ReadUInt32();
                    long dataOffset = reader.ReadUInt32();
                    long ticks = reader.ReadInt64();

                    stringBuilder.Clear();

                    char current;
                    while ((current = reader.ReadChar()) != '\0')
                        stringBuilder.Append(current);

                    stream.Seek(currentOffset + dataOffset, SeekOrigin.Begin);
                    Contents.Add(new DatabaseData
                    {
                        Name = stringBuilder.ToString(),
                        Data = reader.ReadBytes((int)dataSize),
                        Time = ticks != 0 ? new DateTime(ticks) : time
                    });

                    stream.Seek(currentOffset + blockSize, SeekOrigin.Begin);
                }
            }
        }

        public void Load(string filePath)
        {
            if (!filePath.EndsWith(".ar.00", StringComparison.OrdinalIgnoreCase))
            {
                LoadSingle(filePath);
                return;
            }

            filePath = filePath.Substring(0, filePath.Length - 2);

            for (int i = 0;; i++)
            {
                string currentFilePath = $"{filePath}{i:D2}";
                if (!File.Exists(currentFilePath))
                    break;

                LoadSingle(currentFilePath);
            }
        }

        public void Save(string filePath, int padding = 16, int maxSplitSize = 10 * 1024 * 1024)
        {
            bool splitMode = filePath.EndsWith(".ar.00", StringComparison.OrdinalIgnoreCase);

            if (splitMode)
                filePath = filePath.Substring(0, filePath.Length - 2);

            var fileSizes = new List<long>();

            Stream stream = null;
            BinaryWriter writer = null;

            for (var i = 0; i < Contents.Count; i++)
            {
                if (writer == null)
                {
                    stream = File.Create(splitMode ? $"{filePath}{fileSizes.Count:D2}" : filePath);
                    writer = new BinaryWriter(stream, Encoding.UTF8);

                    writer.Write(0);
                    writer.Write(16);
                    writer.Write(20);
                    writer.Write(padding);
                }

                var databaseData = Contents[i];

                long currentOffset = stream.Position;
                long dataOffsetUnaligned = currentOffset + 21 + Encoding.UTF8.GetByteCount(databaseData.Name);
                long dataOffset = (dataOffsetUnaligned + padding - 1) & ~(padding - 1);
                long blockSize = dataOffset + databaseData.Data.Length;

                writer.Write((uint)(blockSize - currentOffset));
                writer.Write(databaseData.Data.Length);
                writer.Write((uint)(dataOffset - currentOffset));
                writer.Write(databaseData.Time.Ticks);
                
                foreach (char c in databaseData.Name)
                    writer.Write(c);

                writer.Write('\0');

                stream.Seek(dataOffset, SeekOrigin.Begin);
                writer.Write(databaseData.Data);

                if ((!splitMode || stream.Length <= maxSplitSize) && i != Contents.Count - 1)
                    continue;

                fileSizes.Add(stream.Length);

                writer.Dispose();
                writer = null;
            }

            if (!splitMode)
                return;

            stream = File.Create(filePath.Substring(0, filePath.Length - 1) + "l");
            writer = new BinaryWriter(stream, Encoding.UTF8);

            writer.Write('A');
            writer.Write('R');
            writer.Write('L');
            writer.Write('2');
            writer.Write((uint)fileSizes.Count);

            foreach (long fileSize in fileSizes)
                writer.Write((uint)fileSize);

            foreach (var databaseData in Contents)
                writer.Write(databaseData.Name);

            writer.Dispose();
        }

        public void Add(DatabaseData databaseData, ConflictPolicy conflictPolicy = ConflictPolicy.RaiseError)
        {
            lock (((ICollection)Contents).SyncRoot)
            {
                var data = Contents.FirstOrDefault(x => x.Name.Equals(databaseData.Name));
                if (data != null)
                {
                    switch (conflictPolicy)
                    {
                        case ConflictPolicy.RaiseError:
                            throw new ArgumentException("Data with same name already exists", nameof(databaseData));

                        case ConflictPolicy.Replace:
                            Contents.Remove(data);
                            break;

                        case ConflictPolicy.Ignore:
                            return;
                    }
                }

                Contents.Add(databaseData);
            }
        }

        public void Sort()
        {
            Contents.Sort((x, y) => string.Compare(x.Name, y.Name, StringComparison.Ordinal));
        }

        public ArchiveDatabase()
        {
            Contents = new List<DatabaseData>();
        }

        public ArchiveDatabase(string filePath) : this()
        {
            Load(filePath);
        }
    }
}