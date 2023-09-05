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

#include <iostream>
#include <time.h>

#include "bp_neuron_net.h"

#include "data_input.h"
#include "neuron_utils.h"

/** indicate 0 ~ 9 */
#define NUM_NET_OUT 10
#define NUM_HIDDEN 200
#define NET_LEARNING_RATE 0.4

#define TRAIN_IMAGES_URL "../docs/train-images.idx3-ubyte"
#define TRAIN_LABELS_URL "../docs/train-labels.idx1-ubyte"

#define TEST_IMANGES_URL "../docs/t10k-images.idx3-ubyte"
#define TEST_LABELS_URL  "../docs/t10k-labels.idx1-ubyte"

typedef std::vector<int> InputIndex;


void ShowNumbersImage(unsigned char pic[], int width, int height)
{
    int idx = 0;
    for (int i=0; i < height; i++)
    {
        for (int j = 0; j < width; j++ )
        {
            if (pic[idx++])
            {
                std::cout << "1";
            }
            else
            {
                std::cout << " ";
            }
        }

        std::cout << std::endl;
    }
}

inline void PreprocessInputData(const unsigned char src[], double out[], int size)
{
    for (int i = 0; i < size; i++)
    {
        out[i] = (src[i] >= 128) ? 1.0 : 0.0;
    }
}

inline void PreprocessInputDataWithNoise(const unsigned char src[], double out[], int size)
{
    for (int i = 0; i < size; i++)
    {
        out[i] = ((src[i] >= 128) ? 1.0 : 0.0) + RandFloat() * 0.1;
    }
}

inline void PreprocessInputData(const unsigned char src[],int size, InputIndex& indexs)
{
    for (int i = 0; i < size; i++)
    {
        if (src[i] >= 128)
        {
            indexs.push_back(i);
        }
    }
}


double TrainEpoch(DataInput& src, BPNeuronNet& bpnn, int imageSize, int numImages)
{
    double net_target[NUM_NET_OUT]{};
    char* temp = new char[imageSize];
    ProgressDisplay progd(numImages);

    double* net_train = new double[imageSize];
    for (int i = 0; i < numImages; i++)
    {
        int label = 0;
        memset(net_target, 0, NUM_NET_OUT * sizeof(double));

        if (src.Read(&label, temp))
        {
            net_target[label] = 1.0;
            PreprocessInputDataWithNoise((unsigned char*)temp, net_train, imageSize);
            bpnn.Train(net_train, net_target);

        }
        else
        {
            std::cout << "read train data failed" << std::endl;
            break;
        }
        //progd.updateProgress(i);

        progd++;
    }

    std::cout << "the error is:" << bpnn.GetError() << " after training " << std::endl;

    delete []net_train;
    delete []temp;

    return bpnn.GetError();
}

int TestRecognition(DataInput& testData, BPNeuronNet& bpnn, int imageSize, int numImages)
{
    int ok_cnt = 0;
    double* net_out = NULL;
    char* temp = new char[imageSize];
    ProgressDisplay progd(numImages);
    double* net_test = new double[imageSize];
    for (int i = 0; i < numImages; i++)
    {
        int label = 0;

        if (testData.Read(&label, temp))
        {			
            PreprocessInputData((unsigned char*)temp, net_test, imageSize);
            bpnn.Process(net_test, &net_out);

            int idx = -1;
            double max_value = -99999;
            for (int i = 0; i < NUM_NET_OUT; i++)
            {
                if (net_out[i] > max_value)
                {
                    max_value = net_out[i];
                    idx = i;
                }
            }

            if (idx == label)
            {
                ok_cnt++;
            }

            progd.UpdateProgress(i);
        }
        else
        {
            std::cout << "read test data failed" << std::endl;
            break;
        }
    }


    delete []net_test;
    delete []temp;

    return ok_cnt;

}


double TrainEpoch2(DataInput& src, BPNeuronNet& bpnn, int imageSize, int numImages)
{
    double net_target[NUM_NET_OUT]{};
    char* temp = new char[imageSize];
    ProgressDisplay progd(numImages);

    InputIndex indexs;

    for (int i = 0; i < numImages; i++)
    {
        int label = 0;
        memset(net_target, 0, NUM_NET_OUT * sizeof(double));
        indexs.clear();

        if (src.Read(&label, temp))
        {
            net_target[label] = 1.0;
            PreprocessInputData((unsigned char*)temp, imageSize, indexs);

            bpnn.Train(indexs.data(), indexs.size(), net_target);

        }
        else
        {
            std::cout << "read train data failed" << std::endl;
            break;
        }
        //progd.updateProgress(i);

        progd++;
    }

    std::cout << "the error is:" << bpnn.GetError() << " after training " << std::endl;

    delete[]temp;

    return bpnn.GetError();
}

int TestRecognition2(DataInput& testData, BPNeuronNet& bpnn, int imageSize, int numImages)
{
    int ok_cnt = 0;
    double* net_out = NULL;
    char* temp = new char[imageSize];
    ProgressDisplay progd(numImages);
    InputIndex indexs;

    for (int i = 0; i < numImages; i++)
    {
        int label = 0;
        indexs.clear();

        if (testData.Read(&label, temp))
        {
            PreprocessInputData((unsigned char*)temp, imageSize, indexs);
            bpnn.Process(indexs.data(), indexs.size(), &net_out);

            int idx = -1;
            double max_value = -99999;
            for (int i = 0; i < NUM_NET_OUT; i++)
            {
                if (net_out[i] > max_value)
                {
                    max_value = net_out[i];
                    idx = i;
                }
            }

            if (idx == label)
            {
                ok_cnt++;
            }

            progd.UpdateProgress(i);
        }
        else
        {
            std::cout << "read test data failed" << std::endl;
            break;
        }
    }

    delete[]temp;

    return ok_cnt;
}

int main(int argc, char* argv[])
{
    DataInput src;
    DataInput testData;
    BPNeuronNet* bpnn = nullptr;
    srand((int)time(0));

    if (src.OpenImageFile(TRAIN_IMAGES_URL) && src.OpenLabelFile(TRAIN_LABELS_URL))
    {
        int imageSize = src.GetImageLength();
        int numImages = src.GetImageCount();
        int epochMax = 1;

        double expectErr = 0.1;

        bpnn = new BPNeuronNet(imageSize, NET_LEARNING_RATE);

        /** add first hidden layer */

        bpnn->AddNeuronLayer(NUM_HIDDEN);
        
        /** add output layer */
        bpnn->AddNeuronLayer(NUM_NET_OUT);

        std::cout << "start training ANN..." << std::endl;
        uint64_t st = GetTimeNowMs();

        for (int i = 0; i < epochMax; i++)
        {
            double err = TrainEpoch(src, *bpnn, imageSize, numImages);

            //if (err <= expectErr)
            {
            //	cout << "train success,the error is: " << err << endl;
            //	break;
            }

            src.Reset();
        }

        std::cout << "training ANN success...cast time: " << (GetTimeNowMs() - st) << "(millisecond)" << std::endl;

        ShowSeparatorLine('=', 80);
        st = GetTimeNowMs();
        
        if (testData.OpenImageFile(TEST_IMANGES_URL) 
            && testData.OpenLabelFile(TEST_LABELS_URL))
        {
            imageSize = testData.GetImageLength();
            numImages = testData.GetImageCount();
            
            std::cout << "start test ANN with t10k images..." << std::endl;

            int ok_cnt = TestRecognition(testData, *bpnn, imageSize, numImages);

            std::cout << "digital recognition cast time:"
                << (GetTimeNowMs() - st) << "(millisecond), " 
                <<  "ok_cnt: " << ok_cnt << ", total: " << numImages << std::endl;
        }
        else
        {
            std::cout << "open test image file failed" << std::endl;
        }
    }
    else
    {
        std::cout << "open train image file failed" << std::endl;
    }

    if (bpnn)
    {
        delete bpnn;
    }

    getchar();

    return 0;
}