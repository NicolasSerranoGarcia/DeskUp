#include "desk_up_error_gui_converter.h"

namespace DeskUp::UI {

int ErrorAdapter::showError(const DeskUp::Error& err){
    const auto [title, icon] = mapLevel(err.level());
    QString message = getUserMessage(err);

    QMessageBox box;
    box.setIcon(icon);
    box.setWindowTitle(title);
    box.setText(message);
    box.setStandardButtons(QMessageBox::Ok);

    return box.exec();
}

std::pair<QString, QMessageBox::Icon> ErrorAdapter::mapLevel(Level lvl)
{
    using Icon = QMessageBox::Icon;
    switch (lvl) {
        case Level::Fatal:   return { "Critical Error", Icon::Critical };
        case Level::Error:   return { "Error", Icon::Critical };
        case Level::Warning: return { "Warning", Icon::Warning };
        case Level::Info:    return { "Information", Icon::Information };
        case Level::Retry:   return { "Retry", Icon::Warning };
        default:             return { "DeskUp", Icon::NoIcon };
    }
}

//todo: append the message of the error to the dialog
QString ErrorAdapter::getUserMessage(const DeskUp::Error& err)
{
    switch (err.type()) {
        case ErrType::AccessDenied:
            return "You do not have permission to perform this operation.";
        case ErrType::FileNotFound:
            return "The specified file or directory could not be found.";
        case ErrType::DiskFull:
            return "There is not enough disk space to complete the operation.";
        case ErrType::InvalidInput:
            return "One or more provided parameters are invalid.";
        case ErrType::Io:
            return "An input/output error occurred while accessing the filesystem.";
        case ErrType::InsufficientMemory:
            return "The system ran out of memory.";
        case ErrType::Timeout:
            return "The operation timed out and was cancelled.";
        case ErrType::Unexpected:
            return "An unexpected error occurred.";
        default:
            return QString::fromUtf8(err.what());
    }
}

}