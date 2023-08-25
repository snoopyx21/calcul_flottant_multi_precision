#include <complex.h>
#include <stdio.h>
#include <mpc.h>

int main()
{
    float whileABC = 1, afora = 1,  dob = 1, if1 = 0, precision, rounding, precision2, rounding2;

    printf("### Testing variables names ###\n");

    // using default pragma extensions
    
    int C2MP___temp_0;
    mpc_t C2MP___temp_1; mpc_init2(C2MP___temp_1, 128);
    mpc_t C2MP___temp_2; mpc_init2(C2MP___temp_2, 128);
    mpc_t C2MP___temp_3; mpc_init2(C2MP___temp_3, 128);
    mpc_t C2MP___temp_4; mpc_init2(C2MP___temp_4, 128);
    mpc_t C2MP___temp_5; mpc_init2(C2MP___temp_5, 128);
    mpc_t C2MP___temp_6; mpc_init2(C2MP___temp_6, 128);
    mpc_t C2MP___temp_7; mpc_init2(C2MP___temp_7, 128);
    int C2MP___temp_8;
    mpc_t C2MP___temp_11; mpc_init2(C2MP___temp_11, 128);
    mpc_t C2MP___temp_12; mpc_init2(C2MP___temp_12, 128);
    mpc_t C2MP___temp_13; mpc_init2(C2MP___temp_13, 128);
    mpc_t C2MP___temp_14; mpc_init2(C2MP___temp_14, 128);
    mpc_t C2MP___temp_18; mpc_init2(C2MP___temp_18, 128);

    mpc_set_d(C2MP___temp_1, afora, MPC_RNDZZ);
    mpc_set_d(C2MP___temp_2, dob, MPC_RNDZZ);
    mpc_add(C2MP___temp_3, C2MP___temp_1, C2MP___temp_2, MPC_RNDZZ);
    whileABC = mpc_get_ldc(C2MP___temp_3, MPC_RNDZZ);
    mpc_set_d(C2MP___temp_3, if1, MPC_RNDZZ);
    mpc_set_d(C2MP___temp_5, whileABC, MPC_RNDZZ);
    mpc_set_si(C2MP___temp_6, 10, MPC_RNDZZ);
    mpc_mul(C2MP___temp_7, C2MP___temp_5, C2MP___temp_6, MPC_RNDZZ);
    C2MP___temp_8 = mpc_cmp(C2MP___temp_3, C2MP___temp_7) == 2;
    C2MP___temp_0 = C2MP___temp_8;
    mpc_set_d(C2MP___temp_7, dob, MPC_RNDZZ);
    mpc_set_d(C2MP___temp_13, whileABC, MPC_RNDZZ);
    mpc_set_si(C2MP___temp_14, 10, MPC_RNDZZ);
    mpc_mul(C2MP___temp_4, C2MP___temp_13, C2MP___temp_14, MPC_RNDZZ);
    
    while (C2MP___temp_0)
    {
        mpc_set_d(C2MP___temp_14, if1, MPC_RNDZZ);
        mpc_add(C2MP___temp_11, C2MP___temp_14, C2MP___temp_7, MPC_RNDZZ);
        if1 = mpc_get_ldc(C2MP___temp_11, MPC_RNDZZ);
        mpc_set_d(C2MP___temp_12, if1, MPC_RNDZZ);
        C2MP___temp_8 = mpc_cmp(C2MP___temp_12, C2MP___temp_4) == 2;
        C2MP___temp_0 = C2MP___temp_8;
    }   // C2MP___temp_0

    mpc_set_d(C2MP___temp_12, dob, MPC_RNDZZ);
    precision2 = mpc_get_ldc(C2MP___temp_12, MPC_RNDZZ);
    mpc_set_d(C2MP___temp_18, precision2, MPC_RNDZZ);
    rounding2 = mpc_get_ldc(C2MP___temp_18, MPC_RNDZZ);

    mpc_clear(C2MP___temp_1);
    mpc_clear(C2MP___temp_2);
    mpc_clear(C2MP___temp_3);
    mpc_clear(C2MP___temp_4);
    mpc_clear(C2MP___temp_5);
    mpc_clear(C2MP___temp_6);
    mpc_clear(C2MP___temp_7);
    mpc_clear(C2MP___temp_11);
    mpc_clear(C2MP___temp_12);
    mpc_clear(C2MP___temp_13);
    mpc_clear(C2MP___temp_14);
    mpc_clear(C2MP___temp_18);


    printf("### result ###\n");
    printf("looped; %lf 20.000000\n", if1);
    printf("whileABC; %lf 2.000000\n", whileABC);
    printf("afora; %lf 1.000000\n", afora);
    printf("dob; %lf 1.000000\n", dob);
    printf("precision2; %lf 1.000000\n", precision2);
    printf("rounding2; %lf 1.000000\n", rounding2);

    return 0;
}