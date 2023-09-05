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

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "data_input.h"

DataInput::DataInput()
	: mLabelLen(0)
	, mImageLen(0)
	, mNumLabel(0)
	, mNumImage(0)
	, mImageStartPos(16)
	, mLableStartPos(8)
	, mImageHeight(0)
	, mImageWidth(0)
{
	//todo something
}

DataInput::~DataInput()
{
	mLabelFile.close();
	mImageFile.close();
}


bool DataInput::OpenLabelFile(const char* url)
{
	mLabelFile.open(url, std::ios::binary | std::ios::in);

	if (mLabelFile.is_open())
	{
		int num = 0;
		char buffer[4]{};

		mLabelFile.read(buffer, 4);

		if (mLabelFile.gcount() == 4)
		{	
			num = GRB4(buffer);
			if (DATA_INPUT_LABEL_FLAG == num)
			{
				mLabelFile.read(buffer, 4);

				if (mLabelFile.gcount() == 4)
				{
					mNumLabel = GRB4(buffer);
					mLabelLen = 1;
					return true;
				}
				else
				{
					std::cout << "read label count failed" << std::endl;
				}
			}
			else
			{
				std::cout << "this file isn't label file,the flag is:" << num << std::endl;
			}
		}
	}

	return false;
}

bool DataInput::OpenImageFile(const char* url)
{
	mImageFile.open(url, std::ios::binary | std::ios::in);

	if (mImageFile.is_open())
	{
		int num = 0;

		char buffer[4]{};

		mImageFile.read(buffer, 4);

		if (mImageFile.gcount() == 4)
		{
			int flag = DATA_INPUT_IMAGE_FLAG;
			num = GRB4(buffer);
			if (DATA_INPUT_IMAGE_FLAG == num)
			{
				mImageFile.read(buffer, 4);

				if (mImageFile.gcount() == 4)
				{
					mNumImage = GRB4(buffer);

					int width = 0;
					int height = 0;

					mImageFile.read((char*)&width, 4);

					if (mImageFile.gcount() == 4)
					{
						width = GRB4(&width);
						mImageFile.read((char*)&height, 4);

						if (mImageFile.gcount() == 4)
						{
							height = GRB4(&height);
							mImageWidth = width;
							mImageHeight = height;
							mImageLen = width * height;
						}
					}

					return true;
				}
				else
				{
					std::cout << "read image count failed" << std::endl;
				}
			}
			else
			{
				std::cout << "this file isn't image file,the flag is:" << num << std::endl;
			}
		}
	}

	return false;
}

bool DataInput::ReadIndex(int* label)
{
	if (mLabelFile.is_open() && !mLabelFile.eof())
	{
		mLabelFile.read((char*)label, mLabelLen);
		return mLabelFile.gcount() == mLabelLen;
	}

	return false;
}

bool DataInput::ReadImage(char imageBuf[])
{
	if (mImageFile.is_open() && !mImageFile.eof())
	{
		mImageFile.read(imageBuf, mImageLen);

		return mImageFile.gcount() == mImageLen;
	}

	return false;
}

bool DataInput::Read(int* label, char imageBuf[])
{
	if (ReadIndex(label))
	{
		return ReadImage(imageBuf);
	}

	return false;
}

void DataInput::Reset()
{
	if (mImageFile.is_open())
	{
		mImageFile.clear();
		mImageFile.seekg(mImageStartPos);
	}

	if (mLabelFile.is_open())
	{
		mLabelFile.clear();
		mLabelFile.seekg(mLableStartPos);
	}
}
