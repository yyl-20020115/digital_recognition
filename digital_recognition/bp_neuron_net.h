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

#ifndef __BP_NEURON_NET_H__
#define __BP_NEURON_NET_H__

#include <vector>

#include "neuron.h"

/** used in bpNeuronNet */

/** range, 0 < x <= 1.0 */
#define ACTIVATION_RESPONSE 0.7

#define BIAS                1


/** bp neuron net */
class BPNeuronNet
{
public:
    BPNeuronNet(int numInputs, double learningRate);
    ~BPNeuronNet();
public:
    inline double GetError(void) { return mErrorSum; }

    bool Train(const double inputs[], const double targets[]);
    void Process(const double inputs[], double* outputs[]);

    void Reset(void);
    void AddNeuronLayer(int numNeurons);

    /** traning with the index array of valid input data */
    bool Train(const int indexArray[], const size_t arraySize, const double targets[]);

    void Process(const int indexArray[], const size_t arraySize, double* outputs[]);
private:


    /** Forward propagation, calculate the output of neuron net */
    inline double SigmoidActive(double activation, double response);
    

    void UpdateNeuronLayer(NeuronLayer& nl, const double inputs[]);

    void UpdateNeuronLayer(NeuronLayer& nl, const int indexArray[], const size_t arraySize);

    /** Back propagation, for training neuron net */
    inline double BackActive(double x);
    void TrainUpdate(const double inputs[], const double targets[]);

    void TrainUpdate(const int indexArray[], const size_t arraySize, const double targets[]);


    void TrainNeuronLayer(NeuronLayer& nl,  const double prevOutActivations[], double prevOutErrors[]);

    /** just for first hidden layer() */
    void TrainNeuronLayer(NeuronLayer& nl, const int indexArray[], const size_t arraySize);
private:
    int mNumInputs;
    int mNumOutputs;
    int mNumHiddenLayers; /** the total layers= mNumHiddenLayers + 1; (doesn't include input layer) */
    double mLearningRate;
    double mErrorSum;
    std::vector<NeuronLayer*> mNeuronLayers;
};

#endif // !__BP_NEURON_NET_H__
