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

#define TRAIN_LABELS_PATH "../docs/train-labels.idx1-ubyte"
#define TRAIN_IMAGES_PATH "../docs/train-images.idx3-ubyte"

#define TEST_LABELS_PATH "../docs/t10k-labels.idx1-ubyte"
#define TEST_IMAGES_PATH "../docs/t10k-images.idx3-ubyte"

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
    char* image_data = new char[imageSize];
    ProgressDisplay progress(numImages);

    double* net_train = new double[imageSize];
    for (int i = 0; i < numImages; i++)
    {
        memset(net_target, 0, NUM_NET_OUT * sizeof(double));
        int label = 0;
        if (src.Read(&label, image_data))
        {
            net_target[label] = 1.0;
            PreprocessInputDataWithNoise((unsigned char*)image_data, net_train, imageSize);
            bpnn.Train(net_train, net_target);

        }
        else
        {
            std::cout << "read train data failed" << std::endl;
            break;
        }
        
        progress++;
    }

    std::cout << "the error is:" << bpnn.GetError() << " after training " << std::endl;

    delete [] net_train;
    delete [] image_data;

    return bpnn.GetError();
}

int TestRecognition(DataInput& testData, BPNeuronNet& bpnn, int imageSize, int numImages)
{
    int ok_cnt = 0;
    double* net_out = nullptr;
    char* image_data = new char[imageSize];
    ProgressDisplay progress(numImages);
    double* net_test = new double[imageSize];
    for (int i = 0; i < numImages; i++)
    {
        int label = 0;

        if (testData.Read(&label, image_data))
        {			
            PreprocessInputData((unsigned char*)image_data, net_test, imageSize);
            bpnn.Process(net_test, &net_out);
            int found_index = -1;
            double max_value = -DBL_MAX;
            //output with max value is the number's index
            //or the number itself
            for (int i = 0; i < NUM_NET_OUT; i++)
            {
                if (net_out[i] > max_value)
                {
                    max_value = net_out[i];
                    found_index = i;
                }
            }

            if (found_index == label)
            {
                ok_cnt++;
            }

            progress.UpdateProgress(i);
        }
        else
        {
            std::cout << "read test data failed" << std::endl;
            break;
        }
    }


    delete [] net_test;
    delete [] image_data;

    return ok_cnt;

}


double TrainEpoch2(DataInput& src, BPNeuronNet& bpnn, int imageSize, int numImages)
{
    double net_target[NUM_NET_OUT]{};
    char* image_data = new char[imageSize];
    ProgressDisplay progress(numImages);

    InputIndex indices;

    for (int i = 0; i < numImages; i++)
    {
        int label = 0;
        memset(net_target, 0, NUM_NET_OUT * sizeof(double));
        indices.clear();

        if (src.Read(&label, image_data))
        {
            net_target[label] = 1.0;
            PreprocessInputData((unsigned char*)image_data, imageSize, indices);

            bpnn.Train(indices.data(), indices.size(), net_target);

        }
        else
        {
            std::cout << "read train data failed" << std::endl;
            break;
        }

        progress++;
    }

    std::cout << "the error is:" << bpnn.GetError() << " after training " << std::endl;

    delete[]image_data;

    return bpnn.GetError();
}

int TestRecognition2(DataInput& testData, BPNeuronNet& bpnn, int imageSize, int numImages)
{
    int ok_cnt = 0;
    double* net_out = nullptr;
    char* image_data = new char[imageSize];
    ProgressDisplay progress(numImages);
    InputIndex indexs;

    for (int i = 0; i < numImages; i++)
    {
        int label = 0;
        indexs.clear();

        if (testData.Read(&label, image_data))
        {
            PreprocessInputData((unsigned char*)image_data, imageSize, indexs);
            bpnn.Process(indexs.data(), indexs.size(), &net_out);

            int idx = -1;
            double max_value = -DBL_MAX;
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

            progress.UpdateProgress(i);
        }
        else
        {
            std::cout << "read test data failed" << std::endl;
            break;
        }
    }

    delete[]image_data;

    return ok_cnt;
}

int main(int argc, char* argv[])
{
    DataInput trainData;
    DataInput testData;
    srand((int)time(0));

    if (trainData.OpenFiles(TRAIN_LABELS_PATH,TRAIN_IMAGES_PATH))
    {
        int imageSize = trainData.GetImageLength();
        int numImages = trainData.GetImageCount();
        int epochMax = 1;

        double expectErr = 0.1;

        BPNeuronNet network(imageSize, NET_LEARNING_RATE);

        /** add first hidden layer */

        network.AddNeuronLayer(NUM_HIDDEN);
        
        /** add output layer */
        network.AddNeuronLayer(NUM_NET_OUT);

        std::cout << "start training ANN..." << std::endl;
        uint64_t st = GetTimeNowMs();

        for (int i = 0; i < epochMax; i++)
        {
            double err = TrainEpoch(trainData, network, imageSize, numImages);

            if (err <= expectErr)
            {
            	std::cout << "train success,the error is: " << err <<std::endl;
            	break;
            }

            trainData.Reset();
        }

        std::cout << "training ANN success...cast time: " << (GetTimeNowMs() - st) << "(millisecond)" << std::endl;

        ShowSeparatorLine('=', 80);
        st = GetTimeNowMs();
        
        if (testData.OpenFiles(TEST_LABELS_PATH,TEST_IMAGES_PATH))
        {
            imageSize = testData.GetImageLength();
            numImages = testData.GetImageCount();
            
            std::cout << "start test ANN with t10k images..." << std::endl;

            int ok_cnt = TestRecognition(testData, network, imageSize, numImages);

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

    getchar();

    return 0;
}