/*
* MIT License
*
* Copyright (c) 2017 wen.gu <454727014@qq.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef __DATA_INPUT_H__
#define __DATA_INPUT_H__

#include <fstream>

#define GRB4(a)  ((unsigned int)((((unsigned char*)(a))[0] << 24) | (((unsigned char*)(a))[1] << 16) |  \
		        (((unsigned char*)(a))[2] <<  8) | ((unsigned char*)(a))[3]))

#define DATA_INPUT_IMAGE_FLAG 0x00000803

#define DATA_INPUT_LABEL_FLAG 0x00000801

class DataInput
{
public:
	DataInput();
	~DataInput();
public:
	void Reset();
	bool OpenLabelFile(const char* url);
	bool OpenImageFile(const char* url);

	bool ReadIndex(int* label);
	bool ReadImage(char imageBuf[]);

	bool Read(int* label, char imageBuf[]);

	inline int GetLabelCount() { return mNumLabel; }
	inline int GetImageCount() { return mNumImage; }

	inline int GetLabelLength() { return mLabelLen; }
	inline int GetImageLength() { return mImageLen; }

	inline int GetImageWidth() { return mImageWidth; }
	inline int GetImageHeight() { return mImageHeight; }
private:
	int mNumLabel;
	int mNumImage;

	int mLabelLen;
	int mImageLen;
	int mImageWidth;
	int mImageHeight;
	int mImageStartPos;
	int mLableStartPos;
	std::fstream mLabelFile;
	std::fstream mImageFile;
};

#endif // !__DATA_INPUT_H__

