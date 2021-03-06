using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.IO.Compression;
using System.Security.Cryptography;

public struct MultiFileFile {

    public byte[] hash;
    public byte[] bytes;
}

public class Program {

    public static void Main(string[] args) {

        if (args.Length < 2) {

            Console.WriteLine("ERROR: Must give a list of files!");
            return;
        }

        List<string> files = new();
        for (int i = 1; i < args.Length; i++) {

            string s = args[i];
            if (s.EndsWith("/*") || s.EndsWith("\\*")) {

                files.AddRange(Directory.GetFiles(s.Remove(s.Length - 1)));
            } else files.Add(Path.GetFileName(s));
        }

        Console.WriteLine($"Making a multifile out of {files.Count} files.");

        List<MultiFileFile> fileList = new();
        using (MD5 hasher = MD5.Create()) {

            for (int i = 0; i < files.Count; i++) {

                Console.WriteLine($"Getting data for file \"{files[i]}\".");
                MultiFileFile file;
                string name = Path.GetFileName(files[i]);
                byte[] hash = hasher.ComputeHash(Encoding.ASCII.GetBytes(name));
                file.hash = hash;
                file.bytes = File.ReadAllBytes(files[i]);
                fileList.Add(file);
            }
        }

        Console.WriteLine("Constructing multifile.");

        const byte NULL = 0x0;

        byte[] dummyFiller = new byte[4] { NULL, NULL, NULL, NULL };
        byte[] endianness = new byte[4] { BitConverter.IsLittleEndian ? (byte)1 : (byte)0, 0, 0, 0 };

        List<byte> buffer1 = new();
        buffer1.InsertRange(0, endianness);

        foreach (MultiFileFile file in fileList) {

            buffer1.AddRange(file.hash);
            buffer1.AddRange(dummyFiller);
            buffer1.AddRange(BitConverter.GetBytes(file.bytes.Length));
        }
        
        buffer1.InsertRange(4, BitConverter.GetBytes(buffer1.Count + 4));

        for (int i = 0; i < fileList.Count; i++) {

            byte[] pointerBytes = BitConverter.GetBytes(buffer1.Count);
            buffer1[i * 24 + 20] = pointerBytes[0];
            buffer1[i * 24 + 21] = pointerBytes[1];
            buffer1[i * 24 + 22] = pointerBytes[2];
            buffer1[i * 24 + 23] = pointerBytes[3];

            buffer1.AddRange(fileList[i].bytes);
            buffer1.Add(NULL); //null terminate the file so it can be read as a C string
        }

        File.WriteAllBytes(Path.GetFullPath(args[0]), buffer1.ToArray());
    }
}
