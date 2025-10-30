#pragma once
#include <string>

namespace Super::Compile::DataProcessor
{
    class DataRecognizer
    {
    private:
        std::wstring _inputFilePath;

    public:
        DataRecognizer(const std::wstring& inputFile);
    };
}