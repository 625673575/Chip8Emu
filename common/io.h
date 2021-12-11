#pragma once
#include <shellscalingapi.h>
#include <Psapi.h>
#include <commdlg.h>
#include <ShlObj_core.h>
#include <comutil.h>
#include "types.h"
namespace io {   
	struct FileDialogFilter
	{
		FileDialogFilter(const std::string& ext_, const std::string& desc_ = {}) : ext(ext_), desc(desc_) {}
		std::string desc;
		std::string ext;
	};

	using FileDialogFilterVec = std::vector<FileDialogFilter>;
	struct FilterSpec
	{
		FilterSpec(const FileDialogFilterVec& filters, bool forOpen)
		{
			size_t size = forOpen ? filters.size() + 1 : filters.size();
			comDlg.reserve(size);
			descs.reserve(size);
			ext.reserve(size);

			if (forOpen) comDlg.push_back({});
			std::wstring all;
			for (const auto& f : filters)
			{
				descs.push_back(string_util::string_2_wstring(f.desc));
				ext.push_back(L"*." + string_util::string_2_wstring(f.ext));
				comDlg.push_back({ descs.back().c_str(), ext.back().c_str() });
				all += ext.back() + L";";
			}

			if (forOpen)
			{
				descs.push_back(L"Supported Formats");
				ext.push_back(all);
				comDlg[0] = { descs.back().c_str(), ext.back().c_str() };
			}
		}

		size_t size() const { return comDlg.size(); }
		const COMDLG_FILTERSPEC* data() const { return comDlg.data(); }
	private:
		std::vector<COMDLG_FILTERSPEC> comDlg;
		std::vector<std::wstring> descs;
		std::vector<std::wstring> ext;
	};

	template<typename DialogType>
	static bool fileDialogCommon(const FileDialogFilterVec& filters, std::string& filename, DWORD options, const CLSID clsid)
	{
		FilterSpec fs(filters, typeid(DialogType) == typeid(IFileOpenDialog));

		DialogType* pDialog;
		CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pDialog));
		pDialog->SetOptions(options | FOS_FORCEFILESYSTEM);
		pDialog->SetFileTypes((uint32_t)fs.size(), fs.data());
		pDialog->SetDefaultExtension(fs.data()->pszSpec);

		if (pDialog->Show(nullptr) == S_OK)
		{
			IShellItem* pItem;
			if (pDialog->GetResult(&pItem) == S_OK)
			{
				PWSTR path;
				if (pItem->GetDisplayName(SIGDN_FILESYSPATH, &path) == S_OK)
				{
					filename = string_util::wstring_2_string(std::wstring(path));
					CoTaskMemFree(path);
					return true;
				}
			}
		}

		return false;
	}

	extern bool saveFileDialog(const FileDialogFilterVec& filters, std::string& filename);

	extern bool openFileDialog(const FileDialogFilterVec& filters, std::string& filename);

	extern bool chooseFolderDialog(std::string& folder);
}