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

#ifndef CO_SIM_IO_H_INCLUDED
#define CO_SIM_IO_H_INCLUDED

/*
This file defines the IO of Kratos-CoSimulation for the exchange of data with external solvers.
Only the delaration of the interface functions is defined in this file,
the corresponding definitions can be found in "impl/co_sim_io_impl.h"

By default the communication is done through files.
Support for additional means of communication can be enabled by uncommenting the following macros.
Note that this introduces dependencies such as e.g. boost (header-only version) or MPI
*/

// macros are protected to avoid redefinition in case they were defined externally (e.g. in CMake)
#ifndef CO_SIM_IO_USING_SOCKETS
// #define CO_SIM_IO_USING_SOCKETS // uncomment for Sockets support
#endif
#ifndef CO_SIM_IO_USING_MPI
// #define CO_SIM_IO_USING_MPI // uncomment for MPI support
#endif

// System includes
#include <string>

// Project includes
#include "impl/define.hpp"
#include "impl/info.hpp"

namespace CoSimIO {

inline Info Connect(const std::string& rConnectionName, CoSimIO::SettingsType Settings);
inline Info Connect(const std::string& rConnectionName, const std::string& rSettingsFileName);

inline Info Disconnect(const std::string& rConnectionName);


template<class TContainerType>
inline Info ImportData(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    TContainerType& rData);

template<class TContainerType>
inline Info ExportData(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    const TContainerType& rData);


template<class TDoubleContainerType, class TIntContainerType>
inline Info ImportMesh(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    TDoubleContainerType& rNodalCoordinates,
    TIntContainerType& rElementConnectivities,
    TIntContainerType& rElementTypes);

template<class TDoubleContainerType, class TIntContainerType>
inline Info ExportMesh(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    const TDoubleContainerType& rNodalCoordinates,
    const TIntContainerType& rElementConnectivities,
    const TIntContainerType& rElementTypes);


inline Info ImportInfo(
    const std::string& rConnectionName,
    Info& rSolutionInfo);

inline Info ExportInfo(
    const std::string& rConnectionName,
    const Info& rSolutionInfo);


inline int IsConverged(const std::string& rConnectionName);

inline Info Run(const std::string& rConnectionName);

template<typename TFunctionType>
inline Info Register(
    const std::string& rConnectionName,
    const std::string& rFunctionName,
    TFunctionType rFunction);

} // namespace CoSimIO

#include "impl/co_sim_io_impl.hpp"

#endif // CO_SIM_IO_H_INCLUDED