#include "io.h"

namespace io {

    bool saveFileDialog(const FileDialogFilterVec& filters, std::string& filename)
    {
        return fileDialogCommon<IFileSaveDialog>(filters, filename, FOS_OVERWRITEPROMPT, CLSID_FileSaveDialog);
    }

    bool openFileDialog(const FileDialogFilterVec& filters, std::string& filename)
    {
        return fileDialogCommon<IFileOpenDialog>(filters, filename, FOS_FILEMUSTEXIST, CLSID_FileOpenDialog);
    }

    bool chooseFolderDialog(std::string& folder)
    {
        return fileDialogCommon<IFileOpenDialog>({}, folder, FOS_PICKFOLDERS | FOS_PATHMUSTEXIST, CLSID_FileOpenDialog);
    }
}