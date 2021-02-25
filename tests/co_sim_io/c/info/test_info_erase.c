/*   ______     _____ _           ________
    / ____/___ / ___/(_)___ ___  /  _/ __ |
   / /   / __ \\__ \/ / __ `__ \ / // / / /
  / /___/ /_/ /__/ / / / / / / // // /_/ /
  \____/\____/____/_/_/ /_/ /_/___/\____/
  Kratos CoSimulationApplication

  License:         BSD License, see license.txt

  Main authors:    Philipp Bucher (https://github.com/philbucher)
*/

/* Project includes */
#include "c/co_sim_io_c.h"

#include "../checks.h"

int main()
{
    /* declaring variables */
    CoSimIO_Info info;

    info = CoSimIO_CreateInfo();

    COSIMIO_CHECK_INT_EQUAL(CoSimIO_Info_Size(info), 0);

    CoSimIO_Info_SetInt(info, "echo_level", 6);
    CoSimIO_Info_SetString(info, "iter", "15");

    COSIMIO_CHECK_INT_EQUAL(CoSimIO_Info_Size(info), 2);

    CoSimIO_Info_Erase(info, "iter");

    COSIMIO_CHECK_INT_EQUAL(CoSimIO_Info_Size(info), 1);

    COSIMIO_CHECK_TRUE(CoSimIO_Info_Has(info, "echo_level"));
    COSIMIO_CHECK_FALSE(CoSimIO_Info_Has(info, "iter"));

    CoSimIO_Info_Erase(info, "non_existing"); /* does not throw by design */

    COSIMIO_CHECK_INT_EQUAL(CoSimIO_Info_Size(info), 1);

    CoSimIO_FreeInfo(info);

    return 0;
}
