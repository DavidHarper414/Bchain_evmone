// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2019 The evmone Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/instructions.h>
#include <evmone/analysis.hpp>
#include <gtest/gtest.h>

TEST(op_table, compare_with_evmc_instruction_tables)
{
    for (int r = EVMC_FRONTIER; r <= EVMC_MAX_REVISION; ++r)
    {
        const auto rev = static_cast<evmc_revision>(r);
        const auto& evmone_tbl = evmone::get_op_table(rev);
        const auto* evmc_tbl = evmc_get_instruction_metrics_table(rev);

        for (size_t i = 0; i < evmone_tbl.size(); ++i)
        {
            const auto& metrics = evmone_tbl[i];
            const auto& ref_metrics = evmc_tbl[i];

            // Compare gas costs. Normalize -1 values in EVMC for undefined instructions.
            EXPECT_EQ(metrics.gas_cost, std::max(ref_metrics.gas_cost, int16_t{0}));

            EXPECT_EQ(metrics.stack_req, ref_metrics.num_stack_arguments);
            EXPECT_EQ(metrics.stack_change,
                ref_metrics.num_stack_returned_items - ref_metrics.num_stack_arguments);
        }
    }
}
