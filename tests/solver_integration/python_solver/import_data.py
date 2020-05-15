#     ______     _____ _           ________
#    / ____/___ / ___/(_)___ ___  /  _/ __ |
#   / /   / __ \\__ \/ / __ `__ \ / // / / /
#  / /___/ /_/ /__/ / / / / / / // // /_/ /
#  \____/\____/____/_/_/ /_/ /_/___/\____/
#  Kratos CoSimulationApplication
#
#  License:         BSD License, see license.txt
#
#  Main authors:    Pooyan Dadvand
#

import CoSimIO

def cosimio_check_equal(a, b):
    assert a == b

# Connection Settings
settings = CoSimIO.Info()
settings.SetString("connection_name", "test_connection") # This must be unique for each connection between two solvers
settings.SetString("solver_name", "my_solver") # Not to be confused with the connection name.
settings.SetInt("echo_level", 1)
settings.SetString("solver_version", "1.25")

# Connecting
return_info = CoSimIO.Connect(settings)
cosimio_check_equal(return_info.GetInt("connection_status"), CoSimIO.ConnectionStatus.Connected)

# Importing data
info = CoSimIO.Info()
info.SetString("identifier", "vector_of_pi")
info.SetString("connection_name", "test_connection")
return_info, received_data = CoSimIO.ImportData(info)

cosimio_check_equal(len(received_data), 4)

for value in received_data:
    cosimio_check_equal(value, 3.14)


# Disconnecting
return_info = CoSimIO.Disconnect(settings)
cosimio_check_equal(return_info.GetInt("connection_status"), CoSimIO.ConnectionStatus.Disconnected)
