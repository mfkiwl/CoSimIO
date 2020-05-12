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

#ifndef CO_SIM_IO_C_INFO_INCLUDED
#define CO_SIM_IO_C_INFO_INCLUDED


// #define CreateNewGetValue(type) \
// type ##type,  const char* Key)

// #define DefineNewGetValue(type) \
// type GetValue##type(char* label, CoSimIO_TransferInfo);

typedef struct CoSimIO_Info
{
    void* PtrCppInfo;
} CoSimIO_Info;


typedef struct CoSimIO_ReturnInfo
{
    void* PtrCppInfo;
} CoSimIO_ReturnInfo;


typedef struct CoSimIO_ConnectionSettings
{
    void* PtrCppInfo;
} CoSimIO_ConnectionSettings;


CoSimIO_Info CoSimIO_CreateInfo();
CoSimIO_ReturnInfo CoSimIO_CreateReturnInfo();
CoSimIO_ConnectionSettings CoSimIO_CreateConnectionSettings();

int CoSimIO_FreeInfo(void* I_Info);

int CoSimIO_Info_Has(const CoSimIO_Info I_Info, const char* I_Key);

int CoSimIO_Info_GetInt(const CoSimIO_Info I_Info, const char* I_Key);
double CoSimIO_Info_GetDouble(const CoSimIO_Info I_Info, const char* I_Key);
int CoSimIO_Info_GetBool(const CoSimIO_Info I_Info, const char* I_Key);

void CoSimIO_Info_SetInt(const CoSimIO_Info I_Info, const char* I_Key, const int I_Value);
void CoSimIO_Info_SetDouble(const CoSimIO_Info I_Info, const char* I_Key, const double I_Value);
void CoSimIO_Info_SetBool(const CoSimIO_Info I_Info, const char* I_Key, const int I_Value);

#endif // CO_SIM_IO_C_INFO_INCLUDED
