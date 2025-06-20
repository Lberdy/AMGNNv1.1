#include"LossFunction.cpp"
#include"Optimizer/ThreadPooling.cpp"

enum Order{
    ORDER_2,
    ORDER_4,
    ORDER_6
};

class Differentiation{

public:
    double h = 0.005;
    Order InterpolationPolynomialOrder = ORDER_4;

private:
    double CalculateFunction(NN& nn, double x, double hc, int layer, int pos, bool W_B, bool output, LossFunction& func, std::vector<std::vector<double>>& InputData, std::vector<std::vector<double>>& OutputData){
        nn.changeValue(layer, pos, W_B, output, x+hc*h);
        double result = func.calculateLoss(nn, InputData, OutputData);
        nn.restoreValues();

        return result;

    }

    double derivateOrder2(NN& nn, double x, int layer, int pos, bool W_B, bool output, LossFunction& func, std::vector<std::vector<double>>& InputData, std::vector<std::vector<double>>& OutputData){

        double f1 = CalculateFunction(nn, x, 1, layer, pos, W_B, output, func, InputData, OutputData);
        double f2 = CalculateFunction(nn, x, -1, layer, pos, W_B, output, func, InputData, OutputData);

        double result = f1 - f2;
        result = result/(2*h);

        return result;

    }

    double derivateOrder4(NN& nn, double x, int layer, int pos, bool W_B, bool output, LossFunction& func, std::vector<std::vector<double>>& InputData, std::vector<std::vector<double>>& OutputData){

        double f1 = CalculateFunction(nn, x, -2, layer, pos, W_B, output, func, InputData, OutputData);
        double f2 = CalculateFunction(nn, x, -1, layer, pos, W_B, output, func, InputData, OutputData);
        double f3 = CalculateFunction(nn, x, 1, layer, pos, W_B, output, func, InputData, OutputData);
        double f4 = CalculateFunction(nn, x, 2, layer, pos, W_B, output, func, InputData, OutputData);

        double result = f1 - 8*f2 + 8*f3 - f4;
        result = result/(12*h);

        return result;

    }

    double derivateOrder6(NN& nn, double x, int layer, int pos, bool W_B, bool output, LossFunction& func, std::vector<std::vector<double>>& InputData, std::vector<std::vector<double>>& OutputData){

        double f1 = CalculateFunction(nn, x, -3, layer, pos, W_B, output, func, InputData, OutputData);
        double f2 = CalculateFunction(nn, x, -2, layer, pos, W_B, output, func, InputData, OutputData);
        double f3 = CalculateFunction(nn, x, -1, layer, pos, W_B, output, func, InputData, OutputData);
        double f4 = CalculateFunction(nn, x, 1, layer, pos, W_B, output, func, InputData, OutputData);
        double f5 = CalculateFunction(nn, x, 2, layer, pos, W_B, output, func, InputData, OutputData);
        double f6 = CalculateFunction(nn, x, 3, layer, pos, W_B, output, func, InputData, OutputData);

        double result = -f1 + 9*f2 - 45*f3 + 45*f4 - 9*f5 + f6;
        result = result/(60*h);

        return result;

    }

public:
    double derivate(NN nn, int layer, int pos, bool W_B, bool output, LossFunction& func, std::vector<std::vector<double>>& InputData, std::vector<std::vector<double>>& OutputData){

        double x;
        
        if(output){
            if(W_B){
                x = nn.outputLayer.InWeights[pos];
            }else{
                x = nn.outputLayer.biases[pos];
            }
        }else{
            if(W_B){
                x = nn.HiddenLayers[layer].InWeights[pos];
            }else{
                x = nn.HiddenLayers[layer].biases[pos];
            }
        }

        double result;

        switch(InterpolationPolynomialOrder){
            case ORDER_2 : result = derivateOrder2(nn, x, layer, pos, W_B, output, func, InputData, OutputData); break;
            case ORDER_4 : result = derivateOrder4(nn, x, layer, pos, W_B, output, func, InputData, OutputData); break;
            case ORDER_6 : result = derivateOrder6(nn, x, layer, pos, W_B, output, func, InputData, OutputData); break;
        }

        return result;

    }

    void save(std::ofstream& fileOut){

        fileOut.write(reinterpret_cast<char*>(&h), sizeof(double));

        int IPO = static_cast<int>(InterpolationPolynomialOrder);
        fileOut.write(reinterpret_cast<char*>(&IPO), sizeof(int));

    }

    void load(std::ifstream& fileIn){

        fileIn.read(reinterpret_cast<char*>(&h), sizeof(double));

        int IPO;
        fileIn.read(reinterpret_cast<char*>(&IPO), sizeof(int));
        InterpolationPolynomialOrder = static_cast<Order>(IPO);

    }

};