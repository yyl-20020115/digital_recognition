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
	, labels()
	, images()
	, firstLabelIndex(0)
	, firstImageIndex(0)
{
}

DataInput::~DataInput()
{
	this->labels.clear();
	for (char* image : this->images) {
		delete[] image;
	}
	this->images.clear();
}

bool DataInput::OpenFiles(const char* labels, const char* images)
{
	return this->OpenLabelFile(labels) && this->OpenImageFile(images);
}

bool DataInput::OpenLabelFile(const char* url)
{
	bool done = false;
	std::fstream mLabelFile;

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
				mLabelFile.read(buffer, sizeof(buffer));

				if (mLabelFile.gcount() == sizeof(buffer))
				{
					mNumLabel = GRB4(buffer);
					mLabelLen = 1;

					for (int i = 0; i < mNumLabel; i++) {
						int label = 0;
						//read 1s
						mLabelFile.read((char*)&label, mLabelLen);
						if (mLabelFile.gcount() == mLabelLen) {
							this->labels.push_back(label);
						}
						if (mLabelFile.eof())break;
					}

					done = true;
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
		mLabelFile.close();
	}

	return done;
}

bool DataInput::OpenImageFile(const char* url)
{
	bool done = false;
	std::fstream mImageFile;
	mImageFile.open(url, std::ios::binary | std::ios::in);

	if (mImageFile.is_open())
	{
		int num = 0;

		char buffer[4]{};

		mImageFile.read(buffer, sizeof(buffer));

		if (mImageFile.gcount() == sizeof(buffer))
		{
			int flag = DATA_INPUT_IMAGE_FLAG;
			num = GRB4(buffer);
			if (DATA_INPUT_IMAGE_FLAG == num)
			{
				mImageFile.read(buffer, sizeof(buffer));

				if (mImageFile.gcount() == sizeof(buffer))
				{
					mNumImage = GRB4(buffer);

					int width = 0;
					int height = 0;

					mImageFile.read((char*)&width, sizeof(width));

					if (mImageFile.gcount() == sizeof(width))
					{
						width = GRB4(&width);
						mImageFile.read((char*)&height,sizeof(height));

						if (mImageFile.gcount() == sizeof(height))
						{
							height = GRB4(&height);
							mImageWidth = width;
							mImageHeight = height;
							mImageLen = width * height;

							for (int i = 0; i < mNumImage; i++) {
								char* imageData = new char[mImageLen];

								mImageFile.read(imageData, mImageLen);
								if (mImageFile.gcount() == mImageLen) {
									this->images.push_back(imageData);
								}
								if (mImageFile.eof()) break;
							}

						}
					}

					done = true;
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
		mImageFile.close();
	}

	return done;
}

bool DataInput::ReadIndex(int* label)
{
	if (label!=nullptr && this->firstLabelIndex<this->labels.size()) {
		*label = this->labels[this->firstLabelIndex++];
		return true;
	}
	return false;
}

bool DataInput::ReadImage(char* imageData, int imageLen)
{
	imageLen = std::min(imageLen, this->mImageLen);
	if (imageLen > 0 && this->firstImageIndex<this->images.size()) {
		memcpy(imageData, this->images[this->firstImageIndex++], imageLen);
		return true;
	}
	return false;
}

bool DataInput::Read(int* label, char* imageData, int imageLen)
{
	return ReadIndex(label) && ReadImage(imageData,imageLen);
}

void DataInput::Reset()
{
	this->firstImageIndex = 0;
	this->firstLabelIndex = 0;
}
