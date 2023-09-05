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

#include "neuron_utils.h"
#include "bp_neuron_net.h"

/************************************************************************
* bp neuron net
************************************************************************/

BPNeuronNet::BPNeuronNet(int numInputs, double learningRate)
    :mNumInputs(numInputs)
    ,mNumOutputs(0)
    ,mNumHiddenLayers(0)
    ,mLearningRate(learningRate)
    ,mErrorSum(9999)
{
}

BPNeuronNet::~BPNeuronNet()
{
    for (size_t i = 0; i < mNeuronLayers.size(); i++)
    {
        if (mNeuronLayers[i]!=nullptr)
        {
            delete mNeuronLayers[i];
            mNeuronLayers[i] = nullptr;
        }
    }

    mNeuronLayers.clear();
}

void BPNeuronNet::Reset()
{
    //for each layer
    for (int i = 0; i < mNumHiddenLayers + 1; ++i)
    {
        mNeuronLayers[i]->Reset();
    }

    mErrorSum = 9999;
}

void BPNeuronNet::AddNeuronLayer(int numNeurons)
{
    int numInputsPerNeuron = (mNeuronLayers.size() > 0) ? mNeuronLayers[mNumHiddenLayers]->mNumNeurons : mNumInputs;

    /** create a neuron layer */
    mNeuronLayers.push_back(new NeuronLayer(numNeurons, numInputsPerNeuron));

    /** calculate the count of hidden layers */
    mNumHiddenLayers = ((int)mNeuronLayers.size() > 0) ? ((int)mNeuronLayers.size() - 1) : 0;
}

/************************************************************************
* bp neuron net forward propagation
************************************************************************/
double BPNeuronNet::SigmoidActive(double activation, double response)
{
    /** sigmoid function: f(x) = 1 /(1 + exp(-x)) */
    return (1.0 / (1.0 + exp(-activation * response)));
}

void BPNeuronNet::UpdateNeuronLayer(NeuronLayer& nl, const double inputs[])
{
    int numNeurons = nl.mNumNeurons;
    int numInputsPerNeuron = nl.mNumInputsPerNeuron;
    double* curOutActivations = nl.mOutActivations;

    //for each neuron
    for (int n = 0; n < numNeurons; ++n)
    {
        double* curWeights = nl.mWeights[n];

        double netinput = 0;
        int k;
        //for each weight
        for (k = 0; k < numInputsPerNeuron; ++k)
        {
            //sum the weights x inputs
            netinput += curWeights[k] * inputs[k];
        }

        //add in the bias
        netinput += curWeights[k] * BIAS;


        //The combined activation is first filtered through the sigmoid 
        //function and a record is kept for each neuron 
        curOutActivations[n] = SigmoidActive(netinput, ACTIVATION_RESPONSE);
    }
}

void BPNeuronNet::UpdateNeuronLayer(NeuronLayer& nl, const int indexArray[], const size_t arraySize)
{
    int numNeurons = nl.mNumNeurons;
    int numInputsPerNeuron = nl.mNumInputsPerNeuron;
    double* curOutActivations = nl.mOutActivations;

    //for each neuron
    for (int n = 0; n < numNeurons; ++n)
    {
        double* curWeights = nl.mWeights[n];

        double netinput = 0;
        //for each weight

        for (size_t k = 0; k < arraySize; k++)
        {
            netinput += curWeights[indexArray[k]];
        }

        //add in the bias
        netinput += curWeights[numInputsPerNeuron] * BIAS;


        //The combined activation is first filtered through the sigmoid 
        //function and a record is kept for each neuron 
        curOutActivations[n] = SigmoidActive(netinput, ACTIVATION_RESPONSE);
    }
}


void BPNeuronNet::Process(const double inputs[], double* outputs[])
{
    for (int i = 0; i < mNumHiddenLayers + 1; i++)
    {
        UpdateNeuronLayer(*mNeuronLayers[i], inputs);
        inputs = mNeuronLayers[i]->mOutActivations;
    }

    *outputs = mNeuronLayers[mNumHiddenLayers]->mOutActivations;

}

void BPNeuronNet::Process(const int indexArray[], const size_t arraySize, double* outputs[])
{

    UpdateNeuronLayer(*mNeuronLayers[0], indexArray, arraySize);

    double* inputs = mNeuronLayers[0]->mOutActivations;


    for (int i = 1; i < mNumHiddenLayers + 1; i++)
    {
        UpdateNeuronLayer(*mNeuronLayers[i], inputs);
        inputs = mNeuronLayers[i]->mOutActivations;
    }

    *outputs = mNeuronLayers[mNumHiddenLayers]->mOutActivations;
}

/************************************************************************
* bp neuron net back propagation
************************************************************************/

double BPNeuronNet::BackActive(double x)
{
    /** calculate the error value with
    * f(x) = x * (1 - x) is the derivatives of sigmoid active function
    */
    return x * (1 - x);
}

void BPNeuronNet::TrainUpdate(const double inputs[], const double targets[])
{
    for (int i = 0; i < mNumHiddenLayers + 1; i++)
    {
        UpdateNeuronLayer(*mNeuronLayers[i], inputs);
        inputs = mNeuronLayers[i]->mOutActivations;
    }

    /** get the activations of output layer */
    NeuronLayer& outLayer = *mNeuronLayers[mNumHiddenLayers];
    double* outActivations = outLayer.mOutActivations;
    double* outErrors = outLayer.mOutErrors;
    int numNeurons = outLayer.mNumNeurons;
    
    mErrorSum = 0;
    /** update the out error of output neuron layer */
    for (int i = 0; i < numNeurons; i++)
    {
        //double err =  outActivations[i] - targets[i];
        double err = targets[i] - outActivations[i];
        outErrors[i] = err;
        /** update the SSE(Sum Squared Error). (when this value becomes lower than a
           *  preset threshold we know the training is successful)
           */
        mErrorSum += err * err;
    }
}

void BPNeuronNet::TrainUpdate(const int indexArray[], const size_t arraySize, const double targets[])
{
    double* inputs;

    UpdateNeuronLayer(*mNeuronLayers[0], indexArray, arraySize);
    inputs = mNeuronLayers[0]->mOutActivations;


    for (int i = 1; i < mNumHiddenLayers + 1; i++)
    {
        UpdateNeuronLayer(*mNeuronLayers[i], inputs);
        inputs = mNeuronLayers[i]->mOutActivations;
    }

    /** get the activations of output layer */
    NeuronLayer& outLayer = *mNeuronLayers[mNumHiddenLayers];
    double* outActivations = outLayer.mOutActivations;
    double* outErrors = outLayer.mOutErrors;
    int numNeurons = outLayer.mNumNeurons;

    mErrorSum = 0;
    /** update the out error of output neuron layer */
    for (int i = 0; i < numNeurons; i++)
    {
        //double err =  outActivations[i] - targets[i];
        double err = targets[i] - outActivations[i];
        outErrors[i] = err;
        /** update the SSE(Sum Squared Error). (when this value becomes lower than a
        *  preset threshold we know the training is successful)
        */
        mErrorSum += err * err;
    }
}


void BPNeuronNet::TrainNeuronLayer(NeuronLayer& nl, const double prevOutActivations[], double prevOutErrors[])
{
    int numNeurons = nl.mNumNeurons;
    int numInputsPerNeuron = nl.mNumInputsPerNeuron;
    double* curOutErrors = nl.mOutErrors;
    double* curOutActivations = nl.mOutActivations;

    /** for each neuron of current layer calculate the error and adjust weights accordingly */

    for (int i = 0; i < numNeurons; i++)
    {
        double* curWeights = nl.mWeights[i];
        double coi = curOutActivations[i];
        /** calculate the error value with  
         * f(x) = x * (1 - x) is the derivatives of sigmoid active function
         */
        double err = curOutErrors[i] * BackActive(coi);

        /** for each weight in this neuron calculate the new weight based
         *  on the error signal and the learning rate
         */

        int w;
        //for each weight up to but not including the bias
        for (w = 0; w < numInputsPerNeuron; w++)
        {
            /** update the output error of prev neuron layer */
            if (prevOutErrors) /** because the input layer only have data, haven't other member */
            {
                prevOutErrors[w] += curWeights[w] * err;
            }	

            /** calculate the new weight based on the back propagation rules */
            curWeights[w] += err * mLearningRate * prevOutActivations[w];
        }

        /** and the bias for this neuron */
        curWeights[w] += err * mLearningRate * BIAS;
    }
}

void BPNeuronNet::TrainNeuronLayer(NeuronLayer& nl, const int indexArray[], const size_t arraySize)
{
    int numNeurons = nl.mNumNeurons;
    int numInputsPerNeuron = nl.mNumInputsPerNeuron;
    double* curOutErrors = nl.mOutErrors;
    double* curOutActivations = nl.mOutActivations;

    /** for each neuron of current layer calculate the error and adjust weights accordingly */

    for (int i = 0; i < numNeurons; i++)
    {
        double* curWeights = nl.mWeights[i];
        double coi = curOutActivations[i];
        /** calculate the error value with
        * f(x) = x * (1 - x) is the derivatives of sigmoid active function
        */
        double err = curOutErrors[i] * BackActive(coi);

        /** for each weight in this neuron calculate the new weight based
        *  on the error signal and the learning rate
        */
        double deltaW = err * mLearningRate;
        //for each weight up to but not including the bias
        for (int w = 0; w < arraySize; w++)
        {
            /** calculate the new weight based on the back propagation rules */
            curWeights[indexArray[w]] += deltaW;
        }

        /** and the bias for this neuron */
        curWeights[numInputsPerNeuron] += err * mLearningRate * BIAS;
    }
}


bool BPNeuronNet::Train(const double inputs[], const double targets[])
{
    const double* prevOutActivations = NULL;
    double* prevOutErrors = NULL;
    TrainUpdate(inputs, targets);

    for (int i = mNumHiddenLayers; i >= 0; i--)
    {
        NeuronLayer& curLayer = *mNeuronLayers[i];

        /** get the out activation of prev layer or use inputs data */

        if (i > 0)
        {
            NeuronLayer& prev = *mNeuronLayers[(i - 1)];
            prevOutActivations = prev.mOutActivations;
            prevOutErrors = prev.mOutErrors;
            memset(prevOutErrors, 0, prev.mNumNeurons * sizeof(double));

        }
        else
        {
            prevOutActivations = inputs;
            prevOutErrors = NULL;
        }

        TrainNeuronLayer(curLayer, prevOutActivations, prevOutErrors);
    }

    return true;
}


bool BPNeuronNet::Train(const int indexArray[], const size_t arraySize, const double targets[])
{
    const double* prevOutActivations = NULL;
    double* prevOutErrors = NULL;
    TrainUpdate(indexArray, arraySize, targets);

    for (int i = mNumHiddenLayers; i >= 0; i--)
    {
        NeuronLayer& curLayer = *mNeuronLayers[i];

        /** get the out activation of prev layer or use inputs data */

        if (i > 0)
        {
            NeuronLayer& prev = *mNeuronLayers[(i - 1)];
            prevOutActivations = prev.mOutActivations;
            prevOutErrors = prev.mOutErrors;
            memset(prevOutErrors, 0, prev.mNumNeurons * sizeof(double));
            TrainNeuronLayer(curLayer, prevOutActivations, prevOutErrors);

        }
        else
        {
            TrainNeuronLayer(curLayer, indexArray, arraySize);
        }        
    }

    return true;
}

