// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2024 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <evmc/evmc.hpp>
#include <evmone/eof.hpp>
#include <evmone/evmone.h>
#include <gtest/gtest.h>
#include <test/utils/bytecode.hpp>

namespace evmone::test
{
using evmc::bytes;

/// Fixture for defining test cases for EOF validation.
///
/// Each test contains multiple cases, which are validated during test teardown.
class eof_validation : public testing::Test
{
protected:
    /// EOF validation test case.
    struct TestCase
    {
        /// Container to be validated.
        bytes container;
        /// Expected error if container is expected to be invalid,
        /// or EOFValidationError::success if it is expected to be valid.
        EOFValidationError error = EOFValidationError::success;
        /// (Optional) Test case description.
        std::string name;
    };

    evmc_revision rev = EVMC_PRAGUE;
    std::vector<TestCase> test_cases;

    /// Adds the case to test cases.
    ///
    /// Can be called as add_test_case(string_view hex, error, name)
    /// or add_test_case(bytes_view cont, error, name).
    void add_test_case(bytecode container, EOFValidationError error, std::string name = {})
    {
        test_cases.push_back({std::move(container), error, std::move(name)});
    }

    /// The test runner.
    void TearDown() override;
};

}  // namespace evmone::test
