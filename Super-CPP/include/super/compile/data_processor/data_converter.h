#pragma once
#include <string>

namespace Super::Compile::DataProcessor
{
    class DataConverter
    {
    private:
        std::wstring _inputFilePath;

    public:
        DataConverter(const std::wstring& inputFile);
    };


}