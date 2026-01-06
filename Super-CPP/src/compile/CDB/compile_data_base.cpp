#include <super/compile/CDB/compile_data_base.h>
#include <super/compile/CDB/encoding.h>
#include <fstream>
#include <super/tool/file.h>
#include <super/compile/global_data.h>
#include <super/error.h>
#include <super/compile/project_config.h>

namespace Super::Compile::CDB::CompileDataBase
{
    void SP::open(const std::wstring& file)
    {
        std::wifstream fs(file, std::ios::binary | std::ios::ate); // 以二进制模式打开文件并定位到文件末尾
        if (!fs)
        {
            SUPER_ERROR_THROW_MESSAGE(L"无法打开文件" + file)
        }

        // 获取文件大小
        std::streamsize fileSize = fs.tellg();
        fs.seekg(0, std::ios::beg);

        // 一次性读取整个文件内容
        std::vector<wchar_t> buffer(fileSize);
        if (!fs.read(buffer.data(), fileSize))
        {
            SUPER_ERROR_THROW_MESSAGE(L"读取文件时出错" + file)
        }

        // 按行处理文件内容
        std::wstring line;
        std::wistringstream stream(buffer.data(), buffer.size());
        while (std::getline(stream, line))
        {
            std::wcout << line << std::endl;
        }
    }

    void SP::saved()
    {
    }

    void SPH::open(const std::wstring& file)
    {
    }

    void SPH::saved()
    {
    }

    void SPM::open(const std::wstring& file)
    {
    }

    void SPM::saved()
    {
    }

    void SPD::open(const std::wstring& file)
    {
    }

    void SPD::saved()
    {
    }

    namespace Operation::MacroList
    {
    }

    namespace Operation::Function
    {
    }

    namespace Operation::Var
    {
    }

    namespace Operation::FunctionArgv
    {
    }

    namespace CDB_LIST
    {
        std::wofstream FILE;
        std::vector<LIST_ITEM> LIST;

        void Init()
        {
            Tool::File::CREATE_DIR_CODE cdc = Tool::File::CreateDir(L"CDB");
            if (cdc == Tool::File::CREATE_DIR_CODE::FAIL_CREATE)
            {
                SUPER_ERROR_THROW_MESSAGE(L"CDB 数据库创建失败")
            }

            FILE.open(L"CDB/CDB_LIST", std::ios::trunc);
            //if (file.is_open()) file.close();
            //file.open(filename, std::ios::app);
        }

        void Colse()
        {
            if (FILE.is_open()) FILE.close();
        }
    }

    void CreateCBDFile(Super::Tool::File::FileType ft, const std::wstring& file)
    {
        CDB_LIST::LIST_ITEM li;
        li.file_type = ft;
        li.file = file;
        li.storage_type = static_cast<CDB_LIST::StorageType>(Super::Compile::Project::ProjectConfig.ProjectStorageType);
        CDB_LIST::LIST.emplace_back(li);

        if (ft == Super::Tool::File::FileType::SP)
        {
            SP* sp = new SP();
            sp->source_file = file;
            Super::Compile::GlobalData::SP_List.emplace_back(sp);
        }
        else if (ft == Super::Tool::File::FileType::SPH)
        {
            SPH* sph = new SPH();
            sph->source_file = file;
            Super::Compile::GlobalData::SPH_List.emplace_back(sph);
        }
        else if (ft == Super::Tool::File::FileType::SPM)
        {
            SPM* spm = new SPM();
            spm->source_file = file;
            Super::Compile::GlobalData::SPM_List.emplace_back(spm);
        }
        else if (ft == Super::Tool::File::FileType::SPD)
        {
            SPD* spd = new SPD();
            spd->source_file = file;
            Super::Compile::GlobalData::SPD_List.emplace_back(spd);
        }
    }

    std::vector<Super::Compile::CDB::CompileDataBase::Macro>* GetMacroList(const std::wstring& file)
    {
        Super::Tool::File::FileType ft = Super::Tool::File::GetFileType(file);
        if (ft == Super::Tool::File::FileType::SP)
        {
            return &GlobalData::GetSP(file)->macro_list;
        }
        else if (ft == Super::Tool::File::FileType::SPH)
        {
            return &GlobalData::GetSPH(file)->macro_list;
        }
        else if (ft == Super::Tool::File::FileType::SPM)
        {
            return &GlobalData::GetSPM(file)->macro_list;
        }
        else
        {
            return &GlobalData::GetSPD(file)->macro_list;
        }
    }

    ImportFile ConvertToImportFile(SP* sp)
    {
        return ImportFile();
    }
}