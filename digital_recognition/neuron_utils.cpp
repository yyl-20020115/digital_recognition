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

#include <chrono>
#include "neuron_utils.h"

ProgressDisplay::ProgressDisplay(int maxValue)
    : mMaxValue(maxValue)
    , mNumStarPer10Percent(5)
    , mProgValue(0)
    , mMaxStarCnt(50)
    , mCurStarCnt(0)
{
    std::cout << "0%";
    for (size_t i = 1; i < 10; i++)
    {
        size_t progValue = i * 10;
        std::cout << std::setfill(' ') << std::setw(mNumStarPer10Percent) << progValue;

        if (i == 9)
        {
            /** 100%  and 50 mush than 2 char, so we need + 2 */
            std::cout << std::setfill(' ') << std::setw(mNumStarPer10Percent + 2) << "100%" << std::endl;
        }
    }

    for (size_t i = 0; i < 10; i++)
    {
        std::cout << '|' << std::setfill('-') << std::setw(mNumStarPer10Percent);
    }
    std::cout << '|' << std::endl;
}

ProgressDisplay::~ProgressDisplay()
{
    std::cout << std::endl;
}

ProgressDisplay& ProgressDisplay::operator++()
{
    UpdateProgress(mProgValue + 1);

    return *this;
}

ProgressDisplay& ProgressDisplay::operator++(int)
{
    UpdateProgress(mProgValue + 1);

    return *this;
}

void ProgressDisplay::UpdateProgress(int progressValue)
{
    mProgValue = progressValue;
    int progValue = mProgValue * mMaxStarCnt / mMaxValue;

    if (progValue > mCurStarCnt)
    {
        std::cout <<'\r' << std::setfill(PROGRESS_DISP_CHAR) << std::setw(progValue) << PROGRESS_DISP_CHAR;
        mCurStarCnt = progValue;

        if (mCurStarCnt >= mMaxStarCnt)
        {
            std::cout << PROGRESS_DISP_CHAR << std::endl;
        }
    }
}

uint64_t GetTimeNowMs()
{
    std::chrono::time_point<std::chrono::system_clock> p2 =
        std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>
        (p2.time_since_epoch()).count();
}
