#include <hal/hal_digital_input.h>

bool SWITCH_get_edumkii_S1()
{
    if(flag_edumkii_S1)
    {
        flag_edumkii_S1 = false;

        // Debounce 10ms
        __delay_cycles(160000);

        return true;
    }
    return false;
}
bool SWITCH_get_edumkii_S2()
{
    if(flag_edumkii_S2)
    {
        flag_edumkii_S2 = false;

        // Debounce 10ms
        __delay_cycles(160000);

        return true;
    }
    return false;
}
