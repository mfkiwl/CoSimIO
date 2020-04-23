// Kratos   ______     _____ _           ________
//         / ____/___ / ___/(_)___ ___  /  _/ __ |
//        / /   / __ \\__ \/ / __ `__ \ / // / / /
//       / /___/ /_/ /__/ / / / / / / // // /_/ /
//       \____/\____/____/_/_/ /_/ /_/___/\____/
//      Kratos CoSimulationApplication
//
//  License:         BSD License, see license.txt
//
//  Main authors:    Philipp Bucher (https://github.com/philbucher)
//

#ifndef CO_SIM_IO_COMM_H_INCLUDED
#define CO_SIM_IO_COMM_H_INCLUDED

// System includes

// Project includes
#include "co_sim_io_internals.h"

namespace CoSimIO {
namespace Internals {

class CoSimCommunication
{
public:
    explicit CoSimCommunication(const std::string& rName, SettingsType& rSettings, const bool IsConnectionMaster) : mrSettings(rSettings),mConnectionName(rName), mIsConnectionMaster(IsConnectionMaster)
    {
        const SettingsType default_settings {
            {"echo_level",   "1"},
            {"print_timing", "0"}
        };
        Internals::AddMissingSettings(default_settings, mrSettings);

        mEchoLevel = std::stoi(mrSettings.at("echo_level"));
        mPrintTiming = (mrSettings.at("print_timing") == "1");
    }

    virtual ~CoSimCommunication() = default; // impl of disconnect has to be in derived class due to order of class destruction

    bool Connect()
    {
        CO_SIM_IO_INFO_IF("CoSimIO", GetEchoLevel()>0) << "Connecting \"" << mConnectionName << "\" as Connection-" << (mIsConnectionMaster ? "MASTER" : "SLAVE") << " ..." << std::endl;

        CO_SIM_IO_ERROR_IF(mIsConnected) << "A connection was already established!" << std::endl;

        mIsConnected = ConnectDetail();

        CO_SIM_IO_ERROR_IF_NOT(mIsConnected) << "Connection was not successful!" << std::endl;

        CO_SIM_IO_INFO_IF("CoSimIO", GetEchoLevel()>0) << "Connection established" << std::endl;

        return mIsConnected;
    }

    bool Disconnect()
    {
        CO_SIM_IO_INFO_IF("CoSimIO", GetEchoLevel()>0) << "Disconnecting \"" << mConnectionName << "\" ..." << std::endl;

        if (mIsConnected) {
            mIsConnected = !DisconnectDetail();
            if (mIsConnected) {
                CO_SIM_IO_INFO("CoSimIO") << "Warning: Disconnect was not successful!" << std::endl;
                return false;
            }
        } else {
            CO_SIM_IO_INFO("CoSimIO") << "Warning: Calling Disconnect but there was no active connection!" << std::endl;
            return false;
        }

        CO_SIM_IO_INFO_IF("CoSimIO", GetEchoLevel()>0) << "Disconnecting successful" << std::endl;

        return true;
    }

    void SendControlSignal(const std::string& rIdentifier, const CoSimIO::ControlSignal Signal)
    {
        CheckConnection(); SendControlSignalDetail(rIdentifier, Signal);
    }
    CoSimIO::ControlSignal RecvControlSignal(std::string& rIdentifier)
    {
        CheckConnection(); return RecvControlSignalDetail(rIdentifier);
    }

    template<class... Args>
    void ImportData(Args&&... args)
    {
        CheckConnection(); ImportDataImpl(std::forward<Args>(args)...);
    }

    template<class... Args>
    void ExportData(Args&&... args)
    {
        CheckConnection(); ExportDataImpl(std::forward<Args>(args)...);
    }

    template<class... Args>
    void ImportMesh(Args&&... args)
    {
        CheckConnection(); ImportMeshImpl(std::forward<Args>(args)...);
    }

    template<class... Args>
    void ExportMesh(Args&&... args)
    {
        CheckConnection(); ExportMeshImpl(std::forward<Args>(args)...);
    }

    template<class... Args>
    void ImportGeometry(Args&&... args)
    {
        CheckConnection(); // ImportGeometryImpl(std::forward<Args>(args)...);
    }

    template<class... Args>
    void ExportGeometry(Args&&... args)
    {
        CheckConnection(); // ExportGeometryImpl(std::forward<Args>(args)...);
    }

protected:
    SettingsType& mrSettings;

    std::string GetConnectionName() const {return mConnectionName;}
    int GetEchoLevel() const              {return mEchoLevel;}
    bool GetIsConnectionMaster() const    {return mIsConnectionMaster;}
    bool GetPrintTiming() const           {return mPrintTiming;}
    bool GetIsConnected() const           {return mIsConnected;}

private:
    std::string mConnectionName;
    int mEchoLevel = 1;
    bool mIsConnectionMaster = false;
    bool mPrintTiming = false;
    bool mIsConnected = false;

    virtual bool ConnectDetail() = 0;
    virtual bool DisconnectDetail() = 0;

    virtual void SendControlSignalDetail(const std::string& rIdentifier, CoSimIO::ControlSignal Signal)
    {
        CO_SIM_IO_ERROR << "SendControlSignalDetail not implemented for this comm-type" << std::endl;
    }
    virtual CoSimIO::ControlSignal RecvControlSignalDetail(std::string& rIdentifier)
    {
        CO_SIM_IO_ERROR << "RecvControlSignalDetail not implemented for this comm-type" << std::endl;
        return CoSimIO::ControlSignal::Dummy;
    }

    virtual void ImportDataImpl(
        const std::string& rIdentifier,
        CoSimIO::Internals::DataContainer<double>& rData)
    {
        CO_SIM_IO_ERROR << "ImportDataImpl not implemented for this comm-type!" << std::endl;
    }

    virtual void ExportDataImpl(
        const std::string& rIdentifier,
        const CoSimIO::Internals::DataContainer<double>& rData)
    {
        CO_SIM_IO_ERROR << "ExportDataImpl not implemented for this comm-type!" << std::endl;
    }

    virtual void ImportMeshImpl(
        const std::string& rIdentifier,
        CoSimIO::Internals::DataContainer<double>& rNodalCoordinates,
        CoSimIO::Internals::DataContainer<int>& rElementConnectivities,
        CoSimIO::Internals::DataContainer<int>& rElementTypes)
    {
        CO_SIM_IO_ERROR << "ImportDataImpl not implemented for this comm-type!" << std::endl;
    }

    virtual void ExportMeshImpl(
        const std::string& rIdentifier,
        const CoSimIO::Internals::DataContainer<double>& rNodalCoordinates,
        const CoSimIO::Internals::DataContainer<int>& rElementConnectivities,
        const CoSimIO::Internals::DataContainer<int>& rElementTypes)
    {
        CO_SIM_IO_ERROR << "ImportDataImpl not implemented for this comm-type!" << std::endl;
    }

    void CheckConnection()
    {
        CO_SIM_IO_ERROR_IF_NOT(mIsConnected) << "No active connection exists!" << std::endl;;
    }
};

} // namespace Internals
} // namespace CoSimIO

#endif // CO_SIM_IO_COMM_H_INCLUDED
