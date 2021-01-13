// LOGOIMGEditor.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <stdio.h>

#define EXIT_MAIN(code,fmt,...) {printf("main() exited. Code %d\nMessage: ",code);printf(fmt,__VA_ARGS__);return code;};

using namespace std;

/*

offsets:
00: 0x00000000 - 0x00004FFF - don't know
01: 0x00005000 - 0x0073FD07 - image 1
(ZEROS)
02: 0x00740000 - 0x00E7AFD7 - image 2
(ZEROS)
03: 0x00E7B000 - 0x015B5FD7 - image 3
(ZEROS)
04: 0x015B6000 - 0x01820000 - image 4

*/

bool FileExist(string file)
{
	fstream io(file.c_str(), ios::in);
	bool ret = io.is_open();
	io.close();
	return ret;
}

int GetOffset(int img, bool front)
{
	switch (img)
	{
	case 1:
		return (front ? 0x00005000 : 0x0073FD07);
	case 2:
		return (front ? 0x00740000 : 0x00E7AFD7);
	case 3:
		return (front ? 0x00E7B000 : 0x015B5FD7);
	case 4:
		return (front ? 0x015B6000 : 0x01820000);
	}
	return 0;
}

int main(int argc,char* argv[])
{
	if (argc != 7)
		EXIT_MAIN(-32767, "This tool take 6 partmers insteaed of %d.\n[Usage] LOGOIMGEditor.exe [old_img_file] [new_img_file] [normal_bmp] [fastboot_bmp] [unlocked_bmp] [destroyed_bmp]\n", argc - 1);
	if (!FileExist(argv[1]))
		EXIT_MAIN(-65534, "Image file \"%s\" does not exist.\n", argv[1]);
	for (int i = 3; i < 7; i++)
		if (!FileExist(argv[i]))
			EXIT_MAIN(-65535, "BMP file \"%s\" does not exist.\n", argv[i]);
	const string input_file_path = argv[1];
	const string output_file_path = argv[2];
	fstream input(input_file_path.c_str(), ios::in | ios::binary);
	if (!input.is_open())
		EXIT_MAIN(-1, "Cannot open input file\n");
	fstream output(output_file_path.c_str(), ios::out | ios::binary);
	if (!output.is_open())
		EXIT_MAIN(-2, "Cannot open output file\n");
	char buffer[1];
	int bytecnt = 0x00004FFF;
	//I don't know the meaning of bytes before 0x00004FFFF, so just copy them.
	printf("Copying bytes before 0x00004FFF......\n");
	for (int i = 0; i < 0x00004FFF; i++)
	{
		input.read(buffer, 1);
		output.write(buffer, 1);
		memset(buffer, 0, sizeof(buffer));
	}
	input.close();
	//Repeat：write every image to the new .img file, and add some zeros bewteen two image.
	for (int i = 3; i < 7; i++)
	{
		printf("Processing image %d (%s)......\n", i - 2, argv[i]);
		input.open(argv[i], ios::in | ios::binary);
		if (!input.is_open())
		{
			output.close();
			EXIT_MAIN(-3, "Cannot open input image \"%s\"\n", argv[i]);
		}
		printf("Filling zeros between images......\n");
		while (bytecnt < GetOffset(i - 2, true))
		{
			memset(buffer, 0, sizeof(buffer));
			output.write(buffer, 1);
			bytecnt++;
		}
		printf("Writing image......\n");
		while (!input.eof())
		{
			input.read(buffer, 1);
			output.write(buffer, 1);
			memset(buffer, 0, sizeof(buffer));
			bytecnt++;
		}
		input.close();
	}
	//fill up zeros in the end of the file.
	printf("Filling zeros at the end of file......\n");
	while (bytecnt < GetOffset(4, false))
	{
		memset(buffer, 0, sizeof(buffer));
		output.write(buffer, 1);
		bytecnt++;
	}
	output.close();
	EXIT_MAIN(0, "Successfully finished.\n");
}