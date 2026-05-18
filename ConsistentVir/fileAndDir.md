# File And Directory

## Create File
```c
int fd = open("foo", O_CREAT | O_WRONLY | O_TRUNC);
```

创建文件：
    1.inode： 存储文件的元数据（metadata），包括文件的大小，在磁盘中的位置，权限等等; struct inode
    2.file<->inode 映射 条目映射

## Access File

```shell
cat foo ~ open + read + write + close

open();
read("foo", buf,size);
write(1, buf,size);
read("foo", "", size);
close(1);
```

## Remove File

rm  的 系统调用 unlink，即--文件的硬连接数，当且仅当link number == 0,才会删除文件内容

## Rename File
mv 的 系统调用 rename，rename是一个**原子操作**,可以实现**File Update Atomic**;

```c
int fd = open("foo.txt.swp", O_CREAT | O_WRONLY); // 新建临时文件
write(fd, buf, size); // 写入数据
int ret = fsync(fd); //强制刷盘
assert(ret == 0);

rename("foo.txt.swp", "foo.txt");
```

为什么能实现**文件状态原子更新呢**?
    1.创建临时文件，所有改动基于临时文件，不会影响原文件
    2.fsync：强制将数据写入到磁盘，确保改动被记录下来，即使系统崩溃也能找到修改的数据
    3.rename：原子性操作，确保原文件数据被临时文件数据覆盖

## fsync

强制将数据写入到磁盘;

write：告诉操作系统，我将要把这个数据写入到文件中，你在合适的时机将数据写入到磁盘; OS会先将数据写入到一个缓冲区之中（内存区域）;

但有时候我们是无法忍受**系统崩溃**的，如果还未写入到磁盘，就直接断电，数据的修改就不会被记录，尤其是在DBMS中;

解决方式：WAL（Write Ahead Log）日志现行机制，在commit之前，使用fsync强制将此次要执行的操作写入到磁盘，这样即使发生崩溃，也指导要修改的数据是什么;

# Directory
## Create Directory

目录：目录本身也是**一个文件**,文件的内容是**file<->inode**的映射条目;

mkdir dir: 不等于创建了一个空目录，默认创建自身引用（.）和父目录引用（..)

```shell
tiny@localhost:~/os-learn/ConsistentVir$ ls -ial
总计 35
141697 drwx------. 2 tiny tiny  3452  5月18日 10:50 .
177435 drwx------. 5 tiny tiny  3452  5月18日 09:57 ..
```

## Access Dir

打印目录内容（file和inode的映射）

```c
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main()
{
	DIR* dp = opendir(".");
	struct dirent* d; // 每一个目录都是一个"目录流"，也是一个dir entry的集合（dp）
	while((d = readdir(dp)) != NULL) // readdir:返回下一个dir entry的指针
	{
		printf("%d %s\n", (int)d->d_ino, (int*)d->d_name);
	}	
	closedir(dp);
	return 0;
}
```

输出结果

```shell
141697 .
177435 ..
93412 a.out
170945 touchFile.c
143127 foo
143203 1.txt
19923 accessDir.c
100341 fileAndDir.md
```

## 链接
* hard link：ln; 普通文件; 和reference number(link number)有关; 多个硬链接指向同一个inode;
* soft link：ln -s; 特殊类型文件; 和link number无关; 文件内容存储"文件路径"，等价于快捷方式; 删除元文件，会出现"dangling reference"

```shell
143203 -rw-------. 2 tiny tiny     6  5月18日 10:10 1-hl.txt
139771 lrwxrwxrwx. 1 tiny tiny     5  5月18日 11:15 1-sl.txt -> 1.txt
143203 -rw-------. 2 tiny tiny     6  5月18日 10:10 1.txt
```

```shell
tiny@localhost:~/os-learn/ConsistentVir$ ln . dir-hl
ln: .: 目录不允许有硬链接
tiny@localhost:~/os-learn/ConsistentVir$ ln -s . dir-sl
```

硬链接的缺陷：
    1.无法为目录创建硬链接，否则会目录成环
    2.无法跨文件系统指向（在C盘无法为D盘的文件创建硬链接）

## 创建并挂载文件系统

mkfs：创建文件系统; 对于一块磁盘，OS要想办法将其格式化为一个**可管理文件数据的数据结构**,形成一个目录树;

创建好文件系统之后，OS还是看不到这个文件系统，因为这个文件系统没有在**OS的目录树下**-->使用mount将文件系统挂载到OS的目录树下，
但要注意的是，mount（挂载）并不是直接将整个文件系统复制到LINUX的目录下，仅仅是挂载，类似于指针，或者说**命名空间重定向**,只是能根据这个路径找到对应文件系统;

**everything under one tree**

使用/作为唯一根目录，在下方挂载其他目录; 是一种磁盘管理的解耦;实现了命名空间的统一;

windows就严格区分且强耦合各个磁盘，分为C/D盘，简单清晰




