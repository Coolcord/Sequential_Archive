# Sequential Archive
A simple archive format that stores small amounts of data sequentially

[![GitHub release](https://img.shields.io/github/release/Coolcord/Sequential_Archive.svg)](https://GitHub.com/Coolcord/Sequential_Archive/releases)
[![Github all releases](https://img.shields.io/github/downloads/Coolcord/Sequential_Archive/total.svg)](https://GitHub.com/Coolcord/Sequential_Archive/releases)
[![GPLv3 license](https://img.shields.io/badge/License-GPLv3-blue.svg)](http://perso.crans.org/besson/LICENSE.html)

# [Download for Windows](https://github.com/Coolcord/Sequential_Archive/releases/download/v1.1.1/SAM.v1.1.1.Setup.exe)

## About Sequential Archives
The Sequential Archive (.sa) format isn't anything ground breaking. It's simply something I wrote for fun
(like most of my software). When I started the project, I was wanting to learn how archive formats work from the ground up.

While Sequential Archives can store any type of file, they are aimed towards storing numerous small text files into one
larger .sa file. Programs can read from a Sequential Archive without unpacking them via a plugin.

## About SAM
SAM (Sequential Archive Manager) is a simple utility that allows users to easily pack and unpack Sequential Archives.
It's very basic, only allowing to pack, unpack, and view Sequential Archives. The viewer is very handy for viewing
text files inside of .sa files.

## FAQ

**Q: Why did you write this?**

A: I wanted something that could take a large amount of small files and store them in a single file. Many disk formats
store files in clusters. For example, NTFS has a cluster size of 4kB by default. This means that if a file is smaller
than 4kB, it will still take up 4kB on the drive. This ends up being a problem if you have a large amount of tiny files.

**Q: Should I be using Sequential Archives instead of .zip/.rar/.7z/etc.?**

A: No! The Sequential Archive format is inferior to these formats! It's not supposed to replace these!

**Q: If I create 2 Sequential Archives using the same files, why do the 2 archive files have a different checksum?**

A: This is due to the "data scrambler" that I built into the .sa format. When a file is created, a scramble
byte is selected and used to scramble the file after packing. Basically, it's a very weak form of encryption.
I wouldn't rely on it for sensitive data, as it's very easy to crack if you know what you're doing.
