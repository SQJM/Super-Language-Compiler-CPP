#pragma once
#include <string>

namespace Super::Compile::DataProcessor
{
    class DataProcessor
    {
    private:
        std::wstring _inputFilePath;

    public:
        DataProcessor(const std::wstring& inputFile);
    };


}