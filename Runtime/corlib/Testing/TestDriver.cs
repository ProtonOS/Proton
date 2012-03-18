using System;

namespace Tester
{
    public class TestDriver
    {
        public static void RunTests()
        {
            #region Tests
            if (Tests.test_1_a_eq_b_plus_a() != 1)
            {
                Console.WriteLine("Test 'test_1_a_eq_b_plus_a' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_a_eq_b_plus_a()).ToString() + "'");
            }
            if (Tests.test_0_comp() != 0)
            {
                Console.WriteLine("Test 'test_0_comp' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_comp()).ToString() + "'");
            }
            if (Tests.test_0_comp_unsigned() != 0)
            {
                Console.WriteLine("Test 'test_0_comp_unsigned' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_comp_unsigned()).ToString() + "'");
            }
            if (Tests.test_16_cmov() != 16)
            {
                Console.WriteLine("Test 'test_16_cmov' didn't return expected value. Expected: '16' Got: '" + ((uint)Tests.test_16_cmov()).ToString() + "'");
            }
            if (Tests.test_0_and_cmp() != 0)
            {
                Console.WriteLine("Test 'test_0_and_cmp' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_and_cmp()).ToString() + "'");
            }
            if (Tests.test_0_mul_imm_opt_Basic() != 0)
            {
                Console.WriteLine("Test 'test_0_mul_imm_opt_Basic' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_mul_imm_opt_Basic()).ToString() + "'");
            }
            if (Tests.test_0_cne() != 0)
            {
                Console.WriteLine("Test 'test_0_cne' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_cne()).ToString() + "'");
            }
            if (Tests.test_0_cmp_regvar_zero() != 0)
            {
                Console.WriteLine("Test 'test_0_cmp_regvar_zero' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_cmp_regvar_zero()).ToString() + "'");
            }
            if (Tests.test_5_div_un_cfold() != 5)
            {
                Console.WriteLine("Test 'test_5_div_un_cfold' didn't return expected value. Expected: '5' Got: '" + ((uint)Tests.test_5_div_un_cfold()).ToString() + "'");
            }
            if (Tests.test_1_rem_un_cfold() != 1)
            {
                Console.WriteLine("Test 'test_1_rem_un_cfold' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_rem_un_cfold()).ToString() + "'");
            }
            if (Tests.test_0_div_opt() != 0)
            {
                Console.WriteLine("Test 'test_0_div_opt' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_div_opt()).ToString() + "'");
            }
            if (Tests.test_0_rem_opt() != 0)
            {
                Console.WriteLine("Test 'test_0_rem_opt' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_rem_opt()).ToString() + "'");
            }
            if (Tests.test_0_branch_to_cmov_opt() != 0)
            {
                Console.WriteLine("Test 'test_0_branch_to_cmov_opt' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_branch_to_cmov_opt()).ToString() + "'");
            }
            if (Tests.test_0_ishr_sign_extend() != 0)
            {
                Console.WriteLine("Test 'test_0_ishr_sign_extend' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_ishr_sign_extend()).ToString() + "'");
            }
            if (Tests.test_0_ishr_sign_extend_cfold() != 0)
            {
                Console.WriteLine("Test 'test_0_ishr_sign_extend_cfold' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_ishr_sign_extend_cfold()).ToString() + "'");
            }
            if (Tests.test_0_rem_imm_0() != 0)
            {
                Console.WriteLine("Test 'test_0_rem_imm_0' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_rem_imm_0()).ToString() + "'");
            }
            if (Tests.test_0_rem_imm_0_neg() != 0)
            {
                Console.WriteLine("Test 'test_0_rem_imm_0_neg' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_rem_imm_0_neg()).ToString() + "'");
            }
            if (Tests.test_4_rem_big_imm() != 4)
            {
                Console.WriteLine("Test 'test_4_rem_big_imm' didn't return expected value. Expected: '4' Got: '" + ((uint)Tests.test_4_rem_big_imm()).ToString() + "'");
            }
            if (Tests.test_9_mul() != 9)
            {
                Console.WriteLine("Test 'test_9_mul' didn't return expected value. Expected: '9' Got: '" + ((uint)Tests.test_9_mul()).ToString() + "'");
            }
            if (Tests.test_15_mul_imm() != 15)
            {
                Console.WriteLine("Test 'test_15_mul_imm' didn't return expected value. Expected: '15' Got: '" + ((uint)Tests.test_15_mul_imm()).ToString() + "'");
            }
            if (Tests.test_24_mul_Basic() != 24)
            {
                Console.WriteLine("Test 'test_24_mul_Basic' didn't return expected value. Expected: '24' Got: '" + ((uint)Tests.test_24_mul_Basic()).ToString() + "'");
            }
            if (Tests.test_24_mul_ovf_Basic() != 24)
            {
                Console.WriteLine("Test 'test_24_mul_ovf_Basic' didn't return expected value. Expected: '24' Got: '" + ((uint)Tests.test_24_mul_ovf_Basic()).ToString() + "'");
            }
            if (Tests.test_24_mul_un_Basic() != 24)
            {
                Console.WriteLine("Test 'test_24_mul_un_Basic' didn't return expected value. Expected: '24' Got: '" + ((uint)Tests.test_24_mul_un_Basic()).ToString() + "'");
            }
            if (Tests.test_24_mul_ovf_un_Basic() != 24)
            {
                Console.WriteLine("Test 'test_24_mul_ovf_un_Basic' didn't return expected value. Expected: '24' Got: '" + ((uint)Tests.test_24_mul_ovf_un_Basic()).ToString() + "'");
            }
            if (Tests.test_0_add_ovf1() != 0)
            {
                Console.WriteLine("Test 'test_0_add_ovf1' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_add_ovf1()).ToString() + "'");
            }
            if (Tests.test_0_add_ovf2() != 0)
            {
                Console.WriteLine("Test 'test_0_add_ovf2' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_add_ovf2()).ToString() + "'");
            }
            if (Tests.test_0_add_ovf3() != 0)
            {
                Console.WriteLine("Test 'test_0_add_ovf3' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_add_ovf3()).ToString() + "'");
            }
            if (Tests.test_0_add_ovf4() != 0)
            {
                Console.WriteLine("Test 'test_0_add_ovf4' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_add_ovf4()).ToString() + "'");
            }
            if (Tests.test_0_add_ovf5() != 0)
            {
                Console.WriteLine("Test 'test_0_add_ovf5' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_add_ovf5()).ToString() + "'");
            }
            if (Tests.test_0_add_ovf6() != 0)
            {
                Console.WriteLine("Test 'test_0_add_ovf6' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_add_ovf6()).ToString() + "'");
            }
            if (Tests.test_0_add_un_ovf_Basic() != 0)
            {
                Console.WriteLine("Test 'test_0_add_un_ovf_Basic' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_add_un_ovf_Basic()).ToString() + "'");
            }
            if (Tests.test_0_sub_ovf1() != 0)
            {
                Console.WriteLine("Test 'test_0_sub_ovf1' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_sub_ovf1()).ToString() + "'");
            }
            if (Tests.test_0_sub_ovf2() != 0)
            {
                Console.WriteLine("Test 'test_0_sub_ovf2' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_sub_ovf2()).ToString() + "'");
            }
            if (Tests.test_0_sub_ovf3() != 0)
            {
                Console.WriteLine("Test 'test_0_sub_ovf3' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_sub_ovf3()).ToString() + "'");
            }
            if (Tests.test_0_sub_ovf4() != 0)
            {
                Console.WriteLine("Test 'test_0_sub_ovf4' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_sub_ovf4()).ToString() + "'");
            }
            if (Tests.test_0_sub_ovf5() != 0)
            {
                Console.WriteLine("Test 'test_0_sub_ovf5' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_sub_ovf5()).ToString() + "'");
            }
            if (Tests.test_0_sub_ovf6() != 0)
            {
                Console.WriteLine("Test 'test_0_sub_ovf6' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_sub_ovf6()).ToString() + "'");
            }
            if (Tests.test_0_sub_ovf_un_Basic() != 0)
            {
                Console.WriteLine("Test 'test_0_sub_ovf_un_Basic' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_sub_ovf_un_Basic()).ToString() + "'");
            }
            if (Tests.test_3_or() != 3)
            {
                Console.WriteLine("Test 'test_3_or' didn't return expected value. Expected: '3' Got: '" + ((uint)Tests.test_3_or()).ToString() + "'");
            }
            if (Tests.test_3_or_un() != 3)
            {
                Console.WriteLine("Test 'test_3_or_un' didn't return expected value. Expected: '3' Got: '" + ((uint)Tests.test_3_or_un()).ToString() + "'");
            }
            if (Tests.test_3_or_short_un() != 3)
            {
                Console.WriteLine("Test 'test_3_or_short_un' didn't return expected value. Expected: '3' Got: '" + ((uint)Tests.test_3_or_short_un()).ToString() + "'");
            }
            if (Tests.test_18_or_imm() != 18)
            {
                Console.WriteLine("Test 'test_18_or_imm' didn't return expected value. Expected: '18' Got: '" + ((uint)Tests.test_18_or_imm()).ToString() + "'");
            }
            if (Tests.test_268435458_or_large_imm() != 268435458)
            {
                Console.WriteLine("Test 'test_268435458_or_large_imm' didn't return expected value. Expected: '268435458' Got: '" + ((uint)Tests.test_268435458_or_large_imm()).ToString() + "'");
            }
            if (Tests.test_268435459_or_large_imm2() != 268435459)
            {
                Console.WriteLine("Test 'test_268435459_or_large_imm2' didn't return expected value. Expected: '268435459' Got: '" + ((uint)Tests.test_268435459_or_large_imm2()).ToString() + "'");
            }
            if (Tests.test_1_xor() != 1)
            {
                Console.WriteLine("Test 'test_1_xor' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_xor()).ToString() + "'");
            }
            if (Tests.test_1_xor_imm() != 1)
            {
                Console.WriteLine("Test 'test_1_xor_imm' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_xor_imm()).ToString() + "'");
            }
            if (Tests.test_983041_xor_imm_large() != 983041)
            {
                Console.WriteLine("Test 'test_983041_xor_imm_large' didn't return expected value. Expected: '983041' Got: '" + ((uint)Tests.test_983041_xor_imm_large()).ToString() + "'");
            }
            if (Tests.test_1_neg() != 1)
            {
                Console.WriteLine("Test 'test_1_neg' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_neg()).ToString() + "'");
            }
            if (Tests.test_2_not() != 2)
            {
                Console.WriteLine("Test 'test_2_not' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_not()).ToString() + "'");
            }
            if (Tests.test_16_shift() != 16)
            {
                Console.WriteLine("Test 'test_16_shift' didn't return expected value. Expected: '16' Got: '" + ((uint)Tests.test_16_shift()).ToString() + "'");
            }
            if (Tests.test_16_shift_add() != 16)
            {
                Console.WriteLine("Test 'test_16_shift_add' didn't return expected value. Expected: '16' Got: '" + ((uint)Tests.test_16_shift_add()).ToString() + "'");
            }
            if (Tests.test_16_shift_add2() != 16)
            {
                Console.WriteLine("Test 'test_16_shift_add2' didn't return expected value. Expected: '16' Got: '" + ((uint)Tests.test_16_shift_add2()).ToString() + "'");
            }
            if (Tests.test_16_shift_imm() != 16)
            {
                Console.WriteLine("Test 'test_16_shift_imm' didn't return expected value. Expected: '16' Got: '" + ((uint)Tests.test_16_shift_imm()).ToString() + "'");
            }
            if (Tests.test_524288_shift_imm_large() != 524288)
            {
                Console.WriteLine("Test 'test_524288_shift_imm_large' didn't return expected value. Expected: '524288' Got: '" + ((uint)Tests.test_524288_shift_imm_large()).ToString() + "'");
            }
            if (Tests.test_12_shift_imm_inv() != 12)
            {
                Console.WriteLine("Test 'test_12_shift_imm_inv' didn't return expected value. Expected: '12' Got: '" + ((uint)Tests.test_12_shift_imm_inv()).ToString() + "'");
            }
            if (Tests.test_12_shift_imm_inv_sbyte() != 12)
            {
                Console.WriteLine("Test 'test_12_shift_imm_inv_sbyte' didn't return expected value. Expected: '12' Got: '" + ((uint)Tests.test_12_shift_imm_inv_sbyte()).ToString() + "'");
            }
            if (Tests.test_1_rshift_imm() != 1)
            {
                Console.WriteLine("Test 'test_1_rshift_imm' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_rshift_imm()).ToString() + "'");
            }
            if (Tests.test_2_unrshift_imm() != 2)
            {
                Console.WriteLine("Test 'test_2_unrshift_imm' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_unrshift_imm()).ToString() + "'");
            }
            if (Tests.test_0_bigunrshift_imm() != 0)
            {
                Console.WriteLine("Test 'test_0_bigunrshift_imm' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_bigunrshift_imm()).ToString() + "'");
            }
            if (Tests.test_0_bigrshift_imm() != 0)
            {
                Console.WriteLine("Test 'test_0_bigrshift_imm' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_bigrshift_imm()).ToString() + "'");
            }
            if (Tests.test_1_rshift() != 1)
            {
                Console.WriteLine("Test 'test_1_rshift' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_rshift()).ToString() + "'");
            }
            if (Tests.test_2_unrshift() != 2)
            {
                Console.WriteLine("Test 'test_2_unrshift' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_unrshift()).ToString() + "'");
            }
            if (Tests.test_0_bigunrshift() != 0)
            {
                Console.WriteLine("Test 'test_0_bigunrshift' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_bigunrshift()).ToString() + "'");
            }
            if (Tests.test_0_bigrshift() != 0)
            {
                Console.WriteLine("Test 'test_0_bigrshift' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_bigrshift()).ToString() + "'");
            }
            if (Tests.test_2_cond() != 2)
            {
                Console.WriteLine("Test 'test_2_cond' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_cond()).ToString() + "'");
            }
            if (Tests.test_2_cond_short() != 2)
            {
                Console.WriteLine("Test 'test_2_cond_short' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_cond_short()).ToString() + "'");
            }
            if (Tests.test_2_cond_sbyte() != 2)
            {
                Console.WriteLine("Test 'test_2_cond_sbyte' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_cond_sbyte()).ToString() + "'");
            }
            if (Tests.test_6_cascade_cond() != 6)
            {
                Console.WriteLine("Test 'test_6_cascade_cond' didn't return expected value. Expected: '6' Got: '" + ((uint)Tests.test_6_cascade_cond()).ToString() + "'");
            }
            if (Tests.test_6_cascade_short() != 6)
            {
                Console.WriteLine("Test 'test_6_cascade_short' didn't return expected value. Expected: '6' Got: '" + ((uint)Tests.test_6_cascade_short()).ToString() + "'");
            }
            if (Tests.test_0_short_sign_extend() != 0)
            {
                Console.WriteLine("Test 'test_0_short_sign_extend' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_short_sign_extend()).ToString() + "'");
            }
            if (Tests.test_127_iconv_to_i1() != 127)
            {
                Console.WriteLine("Test 'test_127_iconv_to_i1' didn't return expected value. Expected: '127' Got: '" + ((uint)Tests.test_127_iconv_to_i1()).ToString() + "'");
            }
            if (Tests.test_384_iconv_to_i2() != 384)
            {
                Console.WriteLine("Test 'test_384_iconv_to_i2' didn't return expected value. Expected: '384' Got: '" + ((uint)Tests.test_384_iconv_to_i2()).ToString() + "'");
            }
            if (Tests.test_15_for_loop() != 15)
            {
                Console.WriteLine("Test 'test_15_for_loop' didn't return expected value. Expected: '15' Got: '" + ((uint)Tests.test_15_for_loop()).ToString() + "'");
            }
            if (Tests.test_11_nested_for_loop() != 11)
            {
                Console.WriteLine("Test 'test_11_nested_for_loop' didn't return expected value. Expected: '11' Got: '" + ((uint)Tests.test_11_nested_for_loop()).ToString() + "'");
            }
            if (Tests.test_11_several_nested_for_loops() != 11)
            {
                Console.WriteLine("Test 'test_11_several_nested_for_loops' didn't return expected value. Expected: '11' Got: '" + ((uint)Tests.test_11_several_nested_for_loops()).ToString() + "'");
            }
            //if (Tests.test_0_conv_ovf_i1() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_conv_ovf_i1' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_conv_ovf_i1()).ToString() + "'");
            //}
            //if (Tests.test_0_conv_ovf_i1_un() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_conv_ovf_i1_un' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_conv_ovf_i1_un()).ToString() + "'");
            //}
            //if (Tests.test_0_conv_ovf_i2() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_conv_ovf_i2' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_conv_ovf_i2()).ToString() + "'");
            //}
            //if (Tests.test_0_conv_ovf_i2_un() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_conv_ovf_i2_un' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_conv_ovf_i2_un()).ToString() + "'");
            //}
            //if (Tests.test_0_conv_ovf_u2() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_conv_ovf_u2' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_conv_ovf_u2()).ToString() + "'");
            //}
            //if (Tests.test_0_conv_ovf_u2_un() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_conv_ovf_u2_un' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_conv_ovf_u2_un()).ToString() + "'");
            //}
            //if (Tests.test_0_conv_ovf_u4() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_conv_ovf_u4' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_conv_ovf_u4()).ToString() + "'");
            //}
            //if (Tests.test_0_conv_ovf_i4_un() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_conv_ovf_i4_un' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_conv_ovf_i4_un()).ToString() + "'");
            //}
            if (Tests.test_0_bool() != 0)
            {
                Console.WriteLine("Test 'test_0_bool' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_bool()).ToString() + "'");
            }
            if (Tests.test_1_bool_inverted() != 1)
            {
                Console.WriteLine("Test 'test_1_bool_inverted' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_bool_inverted()).ToString() + "'");
            }
            if (Tests.test_1_bool_assign() != 1)
            {
                Console.WriteLine("Test 'test_1_bool_assign' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_bool_assign()).ToString() + "'");
            }
            if (Tests.test_1_bool_multi() != 1)
            {
                Console.WriteLine("Test 'test_1_bool_multi' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_bool_multi()).ToString() + "'");
            }
            if (Tests.test_16_spill() != 16)
            {
                Console.WriteLine("Test 'test_16_spill' didn't return expected value. Expected: '16' Got: '" + ((uint)Tests.test_16_spill()).ToString() + "'");
            }
            if (Tests.test_1_switch() != 1)
            {
                Console.WriteLine("Test 'test_1_switch' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_switch()).ToString() + "'");
            }
            if (Tests.test_0_switch_constprop() != 0)
            {
                Console.WriteLine("Test 'test_0_switch_constprop' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_switch_constprop()).ToString() + "'");
            }
            if (Tests.test_0_switch_constprop2() != 0)
            {
                Console.WriteLine("Test 'test_0_switch_constprop2' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_switch_constprop2()).ToString() + "'");
            }
            if (Tests.test_0_while_loop_1() != 0)
            {
                Console.WriteLine("Test 'test_0_while_loop_1' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_while_loop_1()).ToString() + "'");
            }
            if (Tests.test_0_while_loop_2() != 0)
            {
                Console.WriteLine("Test 'test_0_while_loop_2' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_while_loop_2()).ToString() + "'");
            }
            if (Tests.test_0_char_conv() != 0)
            {
                Console.WriteLine("Test 'test_0_char_conv' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_char_conv()).ToString() + "'");
            }
            if (Tests.test_3_shift_regalloc() != 3)
            {
                Console.WriteLine("Test 'test_3_shift_regalloc' didn't return expected value. Expected: '3' Got: '" + ((uint)Tests.test_3_shift_regalloc()).ToString() + "'");
            }
            if (Tests.test_2_optimize_branches() != 2)
            {
                Console.WriteLine("Test 'test_2_optimize_branches' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_optimize_branches()).ToString() + "'");
            }
            //if (Tests.test_0_checked_byte_cast() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_checked_byte_cast' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_checked_byte_cast()).ToString() + "'");
            //}
            //if (Tests.test_0_checked_byte_cast_un() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_checked_byte_cast_un' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_checked_byte_cast_un()).ToString() + "'");
            //}
            //if (Tests.test_0_checked_short_cast() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_checked_short_cast' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_checked_short_cast()).ToString() + "'");
            //}
            //if (Tests.test_0_checked_short_cast_un() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_checked_short_cast_un' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_checked_short_cast_un()).ToString() + "'");
            //}
            if (Tests.test_0_return() != 0)
            {
                Console.WriteLine("Test 'test_0_return' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_return()).ToString() + "'");
            }
            if (Tests.test_2_int_return() != 2)
            {
                Console.WriteLine("Test 'test_2_int_return' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_int_return()).ToString() + "'");
            }
            if (Tests.test_1_int_pass() != 1)
            {
                Console.WriteLine("Test 'test_1_int_pass' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_int_pass()).ToString() + "'");
            }
            if (Tests.test_1_int_pass_many() != 1)
            {
                Console.WriteLine("Test 'test_1_int_pass_many' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_int_pass_many()).ToString() + "'");
            }
            //if (Tests.test_2_inline_saved_arg_type() != 2)
            //{
            //    Console.WriteLine("Test 'test_2_inline_saved_arg_type' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_inline_saved_arg_type()).ToString() + "'");
            //}
            if (Tests.test_5_pass_longs() != 5)
            {
                Console.WriteLine("Test 'test_5_pass_longs' didn't return expected value. Expected: '5' Got: '" + ((uint)Tests.test_5_pass_longs()).ToString() + "'");
            }
            if (Tests.test_55_pass_even_more() != 55)
            {
                Console.WriteLine("Test 'test_55_pass_even_more' didn't return expected value. Expected: '55' Got: '" + ((uint)Tests.test_55_pass_even_more()).ToString() + "'");
            }
            if (Tests.test_1_sparc_argument_passing() != 1)
            {
                Console.WriteLine("Test 'test_1_sparc_argument_passing' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_sparc_argument_passing()).ToString() + "'");
            }
            if (Tests.test_21_sparc_byte_argument_passing() != 21)
            {
                Console.WriteLine("Test 'test_21_sparc_byte_argument_passing' didn't return expected value. Expected: '21' Got: '" + ((uint)Tests.test_21_sparc_byte_argument_passing()).ToString() + "'");
            }
            if (Tests.test_21_sparc_sbyte_argument_passing() != 21)
            {
                Console.WriteLine("Test 'test_21_sparc_sbyte_argument_passing' didn't return expected value. Expected: '21' Got: '" + ((uint)Tests.test_21_sparc_sbyte_argument_passing()).ToString() + "'");
            }
            if (Tests.test_21_sparc_short_argument_passing() != 21)
            {
                Console.WriteLine("Test 'test_21_sparc_short_argument_passing' didn't return expected value. Expected: '21' Got: '" + ((uint)Tests.test_21_sparc_short_argument_passing()).ToString() + "'");
            }
            //if (Tests.test_721_sparc_float_argument_passing() != 721)
            //{
            //    Console.WriteLine("Test 'test_721_sparc_float_argument_passing' didn't return expected value. Expected: '721' Got: '" + ((uint)Tests.test_721_sparc_float_argument_passing()).ToString() + "'");
            //}
            //if (Tests.test_55_sparc_float_argument_passing2() != 55)
            //{
            //    Console.WriteLine("Test 'test_55_sparc_float_argument_passing2' didn't return expected value. Expected: '55' Got: '" + ((uint)Tests.test_55_sparc_float_argument_passing2()).ToString() + "'");
            //}
            //if (Tests.test_0_float_argument_passing_precision() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_float_argument_passing_precision' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_float_argument_passing_precision()).ToString() + "'");
            //}
            //if (Tests.test_2_sparc_takeaddr_argument_passing() != 2)
            //{
            //    Console.WriteLine("Test 'test_2_sparc_takeaddr_argument_passing' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_sparc_takeaddr_argument_passing()).ToString() + "'");
            //}
            //if (Tests.test_721_sparc_takeaddr_argument_passing2() != 721)
            //{
            //    Console.WriteLine("Test 'test_721_sparc_takeaddr_argument_passing2' didn't return expected value. Expected: '721' Got: '" + ((uint)Tests.test_721_sparc_takeaddr_argument_passing2()).ToString() + "'");
            //}
            //if (Tests.test_0_sparc_byref_double_argument_passing() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_sparc_byref_double_argument_passing' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_sparc_byref_double_argument_passing()).ToString() + "'");
            //}
            //if (Tests.test_0_long_arg_assign() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_long_arg_assign' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_long_arg_assign()).ToString() + "'");
            //}
            if (Tests.test_0_ptr_return() != 0)
            {
                Console.WriteLine("Test 'test_0_ptr_return' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_ptr_return()).ToString() + "'");
            }
            //if (Tests.test_0_isnan() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_isnan' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_isnan()).ToString() + "'");
            //}
            if (Tests.test_1_handle_dup_stloc() != 1)
            {
                Console.WriteLine("Test 'test_1_handle_dup_stloc' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_handle_dup_stloc()).ToString() + "'");
            }
            if (Tests.test_3_long_ret() != 3)
            {
                Console.WriteLine("Test 'test_3_long_ret' didn't return expected value. Expected: '3' Got: '" + ((uint)Tests.test_3_long_ret()).ToString() + "'");
            }
            if (Tests.test_1_long_ret2() != 1)
            {
                Console.WriteLine("Test 'test_1_long_ret2' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_long_ret2()).ToString() + "'");
            }
            //if (Tests.test_0_sparc_long_ret_regress_541577() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_sparc_long_ret_regress_541577' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_sparc_long_ret_regress_541577()).ToString() + "'");
            //}
            //if (Tests.test_0_ftol_clobber() != 0)
            //{
            //    Console.WriteLine("Test 'test_0_ftol_clobber' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_ftol_clobber()).ToString() + "'");
            //}
            if (Tests.test_0_return_Basic() != 0)
            {
                Console.WriteLine("Test 'test_0_return_Basic' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_return_Basic()).ToString() + "'");
            }
            if (Tests.test_100000_return_large() != 100000)
            {
                Console.WriteLine("Test 'test_100000_return_large' didn't return expected value. Expected: '100000' Got: '" + ((uint)Tests.test_100000_return_large()).ToString() + "'");
            }
            if (Tests.test_1_load_bool() != 1)
            {
                Console.WriteLine("Test 'test_1_load_bool' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_load_bool()).ToString() + "'");
            }
            if (Tests.test_0_load_bool_false() != 0)
            {
                Console.WriteLine("Test 'test_0_load_bool_false' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_load_bool_false()).ToString() + "'");
            }
            if (Tests.test_200_load_byte() != 200)
            {
                Console.WriteLine("Test 'test_200_load_byte' didn't return expected value. Expected: '200' Got: '" + ((uint)Tests.test_200_load_byte()).ToString() + "'");
            }
            if (Tests.test_100_load_sbyte() != 100)
            {
                Console.WriteLine("Test 'test_100_load_sbyte' didn't return expected value. Expected: '100' Got: '" + ((uint)Tests.test_100_load_sbyte()).ToString() + "'");
            }
            if (Tests.test_200_load_short() != 200)
            {
                Console.WriteLine("Test 'test_200_load_short' didn't return expected value. Expected: '200' Got: '" + ((uint)Tests.test_200_load_short()).ToString() + "'");
            }
            if (Tests.test_100_load_ushort() != 100)
            {
                Console.WriteLine("Test 'test_100_load_ushort' didn't return expected value. Expected: '100' Got: '" + ((uint)Tests.test_100_load_ushort()).ToString() + "'");
            }
            if (Tests.test_3_add_simple() != 3)
            {
                Console.WriteLine("Test 'test_3_add_simple' didn't return expected value. Expected: '3' Got: '" + ((uint)Tests.test_3_add_simple()).ToString() + "'");
            }
            if (Tests.test_3_add_imm() != 3)
            {
                Console.WriteLine("Test 'test_3_add_imm' didn't return expected value. Expected: '3' Got: '" + ((uint)Tests.test_3_add_imm()).ToString() + "'");
            }
            if (Tests.test_13407573_add_largeimm() != 13407573)
            {
                Console.WriteLine("Test 'test_13407573_add_largeimm' didn't return expected value. Expected: '13407573' Got: '" + ((uint)Tests.test_13407573_add_largeimm()).ToString() + "'");
            }
            if (Tests.test_1_sub_simple() != 1)
            {
                Console.WriteLine("Test 'test_1_sub_simple' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_sub_simple()).ToString() + "'");
            }
            if (Tests.test_1_sub_simple_un() != 1)
            {
                Console.WriteLine("Test 'test_1_sub_simple_un' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_sub_simple_un()).ToString() + "'");
            }
            if (Tests.test_1_sub_imm() != 1)
            {
                Console.WriteLine("Test 'test_1_sub_imm' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_sub_imm()).ToString() + "'");
            }
            if (Tests.test_2_sub_large_imm() != 2)
            {
                Console.WriteLine("Test 'test_2_sub_large_imm' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_sub_large_imm()).ToString() + "'");
            }
            if (Tests.test_0_sub_inv_imm() != 0)
            {
                Console.WriteLine("Test 'test_0_sub_inv_imm' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_sub_inv_imm()).ToString() + "'");
            }
            if (Tests.test_2_and() != 2)
            {
                Console.WriteLine("Test 'test_2_and' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_and()).ToString() + "'");
            }
            if (Tests.test_0_and_imm() != 0)
            {
                Console.WriteLine("Test 'test_0_and_imm' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_and_imm()).ToString() + "'");
            }
            if (Tests.test_0_and_large_imm() != 0)
            {
                Console.WriteLine("Test 'test_0_and_large_imm' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_and_large_imm()).ToString() + "'");
            }
            if (Tests.test_0_and_large_imm2() != 0)
            {
                Console.WriteLine("Test 'test_0_and_large_imm2' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_and_large_imm2()).ToString() + "'");
            }
            if (Tests.test_2_div() != 2)
            {
                Console.WriteLine("Test 'test_2_div' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_div()).ToString() + "'");
            }
            if (Tests.test_4_div_imm() != 4)
            {
                Console.WriteLine("Test 'test_4_div_imm' didn't return expected value. Expected: '4' Got: '" + ((uint)Tests.test_4_div_imm()).ToString() + "'");
            }
            if (Tests.test_4_divun_imm() != 4)
            {
                Console.WriteLine("Test 'test_4_divun_imm' didn't return expected value. Expected: '4' Got: '" + ((uint)Tests.test_4_divun_imm()).ToString() + "'");
            }
            if (Tests.test_0_div_fold() != 0)
            {
                Console.WriteLine("Test 'test_0_div_fold' didn't return expected value. Expected: '0' Got: '" + ((uint)Tests.test_0_div_fold()).ToString() + "'");
            }
            if (Tests.test_2_div_fold4() != 2)
            {
                Console.WriteLine("Test 'test_2_div_fold4' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_div_fold4()).ToString() + "'");
            }
            if (Tests.test_2_div_fold16() != 2)
            {
                Console.WriteLine("Test 'test_2_div_fold16' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_div_fold16()).ToString() + "'");
            }
            if (Tests.test_719177_div_destreg() != 719177)
            {
                Console.WriteLine("Test 'test_719177_div_destreg' didn't return expected value. Expected: '719177' Got: '" + ((uint)Tests.test_719177_div_destreg()).ToString() + "'");
            }
            if (Tests.test_1_remun_imm() != 1)
            {
                Console.WriteLine("Test 'test_1_remun_imm' didn't return expected value. Expected: '1' Got: '" + ((uint)Tests.test_1_remun_imm()).ToString() + "'");
            }
            if (Tests.test_2_bigremun_imm() != 2)
            {
                Console.WriteLine("Test 'test_2_bigremun_imm' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_bigremun_imm()).ToString() + "'");
            }
            if (Tests.test_2_rem_Basic() != 2)
            {
                Console.WriteLine("Test 'test_2_rem_Basic' didn't return expected value. Expected: '2' Got: '" + ((uint)Tests.test_2_rem_Basic()).ToString() + "'");
            }
            if (Tests.test_4_rem_imm() != 4)
            {
                Console.WriteLine("Test 'test_4_rem_imm' didn't return expected value. Expected: '4' Got: '" + ((uint)Tests.test_4_rem_imm()).ToString() + "'");
            }
            #endregion
        }

        public static void WriteError(string s)
        {
            Console.WriteLine("Error: " + s);
        }
    }
}

