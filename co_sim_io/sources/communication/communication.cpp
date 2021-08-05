//     ______     _____ _           ________
//    / ____/___ / ___/(_)___ ___  /  _/ __ |
//   / /   / __ \\__ \/ / __ `__ \ / // / / /
//  / /___/ /_/ /__/ / / / / / / // // /_/ /
//  \____/\____/____/_/_/ /_/ /_/___/\____/
//  Kratos CoSimulationApplication
//
//  License:         BSD License, see license.txt
//
//  Main authors:    Philipp Bucher (https://github.com/philbucher)
//

// System includes
#include <thread>
#include <system_error>

// Project includes
#include "includes/communication/communication.hpp"
#include "includes/utilities.hpp"
#include "includes/version.hpp"

namespace CoSimIO {
namespace Internals {

Communication::Communication(
    const Info& I_Settings,
    std::shared_ptr<DataCommunicator> I_DataComm)
    : mpDataComm(I_DataComm),
      mMyName(I_Settings.Get<std::string>("my_name")),
      mConnectTo(I_Settings.Get<std::string>("connect_to")),
      mWorkingDirectory(I_Settings.Get<std::string>("working_directory", fs::relative(fs::current_path()).string())),
      mEchoLevel(I_Settings.Get<int>("echo_level", 0)),
      mPrintTiming(I_Settings.Get<bool>("print_timing", false))
{
    CO_SIM_IO_TRY

    if (I_Settings.Has("is_primary_connection")) {
        mIsPrimaryConnection = I_Settings.Get<bool>("is_primary_connection");
        mPrimaryWasExplicitlySpecified = true;
    } else {
        // automatically determine the primary connection in case the user didn't specify it
        mIsPrimaryConnection = mMyName < mConnectTo;
        mPrimaryWasExplicitlySpecified = false;
    }
    mConnectionName = CreateConnectionName(mMyName, mConnectTo);

    CO_SIM_IO_ERROR_IF_NOT(fs::exists(mWorkingDirectory)) << "The working directory " << mWorkingDirectory << " does not exist!" << std::endl;

    mCommFolder = GetWorkingDirectory();
    mCommFolder /= ".CoSimIOFileComm_" + GetConnectionName();
    mCommInFolder = I_Settings.Get<bool>("use_folder_for_communication", true);

    CO_SIM_IO_CATCH
}

Info Communication::Connect(const Info& I_Info)
{
    CO_SIM_IO_TRY

    CO_SIM_IO_INFO_IF("CoSimIO", GetEchoLevel()>0)
        << "Establishing connection for \"" << mConnectionName
        << "\"\n    from: \"" << mMyName
        << "\"\n    to:   \"" << mConnectTo
        << "\"\n    as " << (mIsPrimaryConnection ? "PRIMARY" : "SECONDARY")
        << " connection; working directory: " << mWorkingDirectory << " ..." << std::endl;

    CO_SIM_IO_ERROR_IF(mIsConnected) << "A connection was already established!" << std::endl;

    Info connect_detail_info = ConnectDetail(I_Info);
    mIsConnected = connect_detail_info.Get<bool>("is_connected");
    connect_detail_info.Set<int>("connection_status", ConnectionStatus::Connected);
    connect_detail_info.Set<std::string>("working_directory", mWorkingDirectory.string());

    CO_SIM_IO_ERROR_IF_NOT(mIsConnected) << "Connection was not successful!" << std::endl;

    CO_SIM_IO_INFO_IF("CoSimIO", GetEchoLevel()>0) << "Connection established" << std::endl;

    PerformCompatibilityCheck();

    return connect_detail_info;

    CO_SIM_IO_CATCH
}

Info Communication::Disconnect(const Info& I_Info)
{
    CO_SIM_IO_TRY

    CO_SIM_IO_INFO_IF("CoSimIO", GetEchoLevel()>0) << "Disconnecting \"" << mConnectionName << "\" ..." << std::endl;

    if (mIsConnected) {
        Info disconnect_detail_info = DisconnectDetail(I_Info);
        mIsConnected = disconnect_detail_info.Get<bool>("is_connected");

        if (mIsConnected) {
            CO_SIM_IO_INFO("CoSimIO") << "Warning: Disconnect was not successful!" << std::endl;
            disconnect_detail_info.Set<int>("connection_status", ConnectionStatus::DisconnectionError);
        } else {
            CO_SIM_IO_INFO_IF("CoSimIO", GetEchoLevel()>0) << "Disconnecting successful" << std::endl;
            disconnect_detail_info.Set<int>("connection_status", ConnectionStatus::Disconnected);
        }
        return disconnect_detail_info;

    } else {
        CO_SIM_IO_INFO("CoSimIO") << "Warning: Calling Disconnect but there was no active connection!" << std::endl;
        Info disconnect_info;
        disconnect_info.Set<bool>("is_connected", false);
        disconnect_info.Set<int>("connection_status", ConnectionStatus::DisconnectionError);
        return disconnect_info;
    }

    CO_SIM_IO_CATCH
}

Info Communication::ConnectDetail(const Info& I_Info)
{
    CO_SIM_IO_TRY

    if (mCommInFolder) {
        if (GetIsPrimaryConnection()) {
            // delete and recreate directory to remove potential leftovers
            std::error_code ec;
            fs::remove_all(mCommFolder, ec);
            if (ec) {
                CO_SIM_IO_INFO("CoSimIO") << "Warning, communication directory (" << mCommFolder << ")could not be deleted!\nError code: " << ec.message() << std::endl;
            }
            if (!fs::exists(mCommFolder)) {
                fs::create_directory(mCommFolder);
            }
        }
    }

    ExchangeSyncFileWithPartner("connect");

    Info info;
    info.Set("is_connected", true);
    return info;

    CO_SIM_IO_CATCH
}

Info Communication::DisconnectDetail(const Info& I_Info)
{
    CO_SIM_IO_TRY

    ExchangeSyncFileWithPartner("disconnect");

    if (mCommInFolder && GetIsPrimaryConnection()) {
        // delete directory to remove potential leftovers
        std::error_code ec;
        fs::remove_all(mCommFolder, ec);
        if (ec) {
            CO_SIM_IO_INFO("CoSimIO") << "Warning, communication directory (" << mCommFolder << ")could not be deleted!\nError code: " << ec.message() << std::endl;
        }
    }

    Info info;
    info.Set("is_connected", false);
    return info;

    CO_SIM_IO_CATCH
}

fs::path Communication::GetTempFileName(const fs::path& rPath) const
{
    CO_SIM_IO_TRY

    if (mCommInFolder) {
        return rPath.string().insert(mCommFolder.string().length()+1, ".");
    } else {
        return "." + rPath.string();
    }

    CO_SIM_IO_CATCH
}

fs::path Communication::GetFileName(const fs::path& rPath, const std::string& rExtension) const
{
    CO_SIM_IO_TRY

    fs::path local_copy(rPath);
    local_copy += "." + rExtension;

    if (mCommInFolder) {
        return mCommFolder / local_copy;
    } else {
        return local_copy;
    }

    CO_SIM_IO_CATCH
}

void Communication::WaitForPath(const fs::path& rPath) const
{
    CO_SIM_IO_TRY

    CO_SIM_IO_INFO_IF("CoSimIO", GetEchoLevel()>0) << "Waiting for: " << rPath << std::endl;
    while(!fs::exists(rPath)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // wait 0.001s before next check
    }
    CO_SIM_IO_INFO_IF("CoSimIO", GetEchoLevel()>0) << "Found: " << rPath << std::endl;

    CO_SIM_IO_CATCH
}

void Communication::WaitUntilFileIsRemoved(const fs::path& rPath) const
{
    CO_SIM_IO_TRY

    if (fs::exists(rPath)) { // only issue the wating message if the file exists initially
        CO_SIM_IO_INFO_IF("CoSimIO", GetEchoLevel()>0) << "Waiting for: " << rPath << " to be removed" << std::endl;
        while(fs::exists(rPath)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5)); // wait 0.001s before next check
        }
        CO_SIM_IO_INFO_IF("CoSimIO", GetEchoLevel()>0) << rPath << " was removed" << std::endl;
    }

    CO_SIM_IO_CATCH
}

void Communication::MakeFileVisible(const fs::path& rPath) const
{
    CO_SIM_IO_TRY

    std::error_code ec;
    fs::rename(GetTempFileName(rPath), rPath, ec);
    CO_SIM_IO_ERROR_IF(ec) << rPath << " could not be made visible!\nError code: " << ec.message() << std::endl;

    CO_SIM_IO_CATCH
}

void Communication::RemovePath(const fs::path& rPath) const
{
    CO_SIM_IO_TRY

    // In windows the file cannot be removed if another file handle is using it
    // this can be the case here if the partner checks if the file (still) exists
    // hence we try multiple times to delete it
    std::error_code ec;
    for (std::size_t i=0; i<5; ++i) {
        if (fs::remove(rPath, ec)) {
            return; // if file could be removed succesfully then return
        }
    }
    CO_SIM_IO_ERROR << rPath << " could not be deleted!\nError code: " << ec.message() << std::endl;

    CO_SIM_IO_CATCH
}

void Communication::ExchangeSyncFileWithPartner(const std::string& rIdentifier) const
{
    CO_SIM_IO_TRY

    const fs::path file_name_primary(GetFileName("CoSimIO_primary_" + rIdentifier + "_" + GetConnectionName(), "sync"));
    const fs::path file_name_secondary(GetFileName("CoSimIO_secondary_" + rIdentifier + "_" + GetConnectionName(), "sync"));

    if (GetIsPrimaryConnection()) {
        std::ofstream sync_file;
        sync_file.open(GetTempFileName(file_name_primary));
        sync_file.close();
        CO_SIM_IO_ERROR_IF_NOT(fs::exists(GetTempFileName(file_name_primary))) << "Primary sync file " << file_name_primary << " could not be created!" << std::endl;
        MakeFileVisible(file_name_primary);

        WaitForPath(file_name_secondary);
        RemovePath(file_name_secondary);

        WaitUntilFileIsRemoved(file_name_primary);
    } else {
        WaitForPath(file_name_primary);
        RemovePath(file_name_primary);

        std::ofstream sync_file;
        sync_file.open(GetTempFileName(file_name_secondary));
        sync_file.close();
        CO_SIM_IO_ERROR_IF_NOT(fs::exists(GetTempFileName(file_name_secondary))) << "Secondary sync file " << file_name_secondary << " could not be created!" << std::endl;
        MakeFileVisible(file_name_secondary);

        WaitUntilFileIsRemoved(file_name_secondary);
    }

    CO_SIM_IO_CATCH
}

void Communication::PerformCompatibilityCheck()
{
    CO_SIM_IO_TRY

    CoSimIO::Info my_info;
    CoSimIO::Info partner_info;
    my_info.Set<int>("version_major", GetMajorVersion());
    my_info.Set<int>("version_minor", GetMinorVersion());
    my_info.Set("version_patch", GetPatchVersion());
    my_info.Set<bool>("primary_was_explicitly_specified", mPrimaryWasExplicitlySpecified);

    if (GetIsPrimaryConnection()) {
        my_info.Set("identifier", "compatibility_checks_1");
        ExportInfo(my_info);
        CoSimIO::Info partner_import_info;
        partner_import_info.Set("identifier", "compatibility_checks_2");
        partner_info = ImportInfo(partner_import_info);
    } else {
        CoSimIO::Info partner_import_info;
        partner_import_info.Set("identifier", "compatibility_checks_1");
        partner_info = ImportInfo(partner_import_info);
        my_info.Set("identifier", "compatibility_checks_2");
        ExportInfo(my_info);
    }

    // perform checks for compatibility
    CO_SIM_IO_ERROR_IF(GetMajorVersion() != partner_info.Get<int>("version_major")) << "Major version mismatch! My version: " << GetMajorVersion() << "; partner version: " << partner_info.Get<int>("version_major") << std::endl;
    CO_SIM_IO_ERROR_IF(GetMinorVersion() != partner_info.Get<int>("version_minor")) << "Minor version mismatch! My version: " << GetMinorVersion() << "; partner version: " << partner_info.Get<int>("version_minor") << std::endl;
    CO_SIM_IO_ERROR_IF(mPrimaryWasExplicitlySpecified != partner_info.Get<bool>("primary_was_explicitly_specified")) << std::boolalpha << "Mismatch in how the primary connection was specified!\nPrimary connection was explicitly specified for me: " << mPrimaryWasExplicitlySpecified << "\nPrimary connection was explicitly specified for partner: " << partner_info.Get<bool>("primary_was_explicitly_specified") << std::endl;

    CO_SIM_IO_CATCH
}

} // namespace Internals
} // namespace CoSimIO