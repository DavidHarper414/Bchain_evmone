// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2025 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include <benchmark/benchmark.h>
#include <evmone/constants.hpp>
#include <evmone/eof.hpp>
#include <test/utils/bytecode.hpp>

namespace
{
using namespace evmone::test;

const bytes max_code_sections = [] {
    auto eof_code_sections_1023 = eof_bytecode(jumpf(1));
    for (int i = 1; i < 1022; ++i)
        eof_code_sections_1023 =
            eof_code_sections_1023.code(jumpf(static_cast<uint16_t>(i + 1)), 0, 0x80, 0);

    auto eof_code_sections_1024 = eof_code_sections_1023;
    eof_code_sections_1023 = eof_code_sections_1023.code(OP_STOP, 0, 0x80, 0);
    eof_code_sections_1024 = eof_code_sections_1024.code(jumpf(1023), 0, 0x80, 0);
    eof_code_sections_1024 = eof_code_sections_1024.code(OP_STOP, 0, 0x80, 0);
    return eof_code_sections_1024;
}();

const bytes max_nested_containers = [] {
    bytecode code{};
    bytecode nextcode = eof_bytecode(OP_INVALID);
    while (nextcode.size() <= evmone::MAX_INITCODE_SIZE)
    {
        code = nextcode;
        nextcode = eof_bytecode(4 * push0() + OP_EOFCREATE + Opcode{0} + OP_INVALID, 4)
                       .container(nextcode);
    }
    return code;
}();

const bytes max_nested_containers2 = [] {
    bytecode code{};
    bytecode nextcode = eof_bytecode(OP_INVALID);
    while (nextcode.size() <= evmone::MAX_INITCODE_SIZE)
    {
        code = nextcode;

        const bytecode initcode =
            eof_bytecode(push0() + push0() + OP_RETURNCONTRACT + Opcode{0}, 2).container(nextcode);
        if (initcode.size() >= std::numeric_limits<uint16_t>::max())
            break;
        nextcode = eof_bytecode(4 * push0() + OP_EOFCREATE + Opcode{0} + OP_INVALID, 4)
                       .container(initcode);
    }
    return code;
}();

const bytes stack_height_max_span = evmc::from_spaced_hex(
    "ef000101000402000113fc04000000008003ff5fe113f75f5fe113f25f5fe113ed5f5fe113e85f5fe113e35f5fe113"
    "de5f5fe113d95f5fe113d45f5fe113cf5f5fe113ca5f5fe113c55f5fe113c05f5fe113bb5f5fe113b65f5fe113b15f"
    "5fe113ac5f5fe113a75f5fe113a25f5fe1139d5f5fe113985f5fe113935f5fe1138e5f5fe113895f5fe113845f5fe1"
    "137f5f5fe1137a5f5fe113755f5fe113705f5fe1136b5f5fe113665f5fe113615f5fe1135c5f5fe113575f5fe11352"
    "5f5fe1134d5f5fe113485f5fe113435f5fe1133e5f5fe113395f5fe113345f5fe1132f5f5fe1132a5f5fe113255f5f"
    "e113205f5fe1131b5f5fe113165f5fe113115f5fe1130c5f5fe113075f5fe113025f5fe112fd5f5fe112f85f5fe112"
    "f35f5fe112ee5f5fe112e95f5fe112e45f5fe112df5f5fe112da5f5fe112d55f5fe112d05f5fe112cb5f5fe112c65f"
    "5fe112c15f5fe112bc5f5fe112b75f5fe112b25f5fe112ad5f5fe112a85f5fe112a35f5fe1129e5f5fe112995f5fe1"
    "12945f5fe1128f5f5fe1128a5f5fe112855f5fe112805f5fe1127b5f5fe112765f5fe112715f5fe1126c5f5fe11267"
    "5f5fe112625f5fe1125d5f5fe112585f5fe112535f5fe1124e5f5fe112495f5fe112445f5fe1123f5f5fe1123a5f5f"
    "e112355f5fe112305f5fe1122b5f5fe112265f5fe112215f5fe1121c5f5fe112175f5fe112125f5fe1120d5f5fe112"
    "085f5fe112035f5fe111fe5f5fe111f95f5fe111f45f5fe111ef5f5fe111ea5f5fe111e55f5fe111e05f5fe111db5f"
    "5fe111d65f5fe111d15f5fe111cc5f5fe111c75f5fe111c25f5fe111bd5f5fe111b85f5fe111b35f5fe111ae5f5fe1"
    "11a95f5fe111a45f5fe1119f5f5fe1119a5f5fe111955f5fe111905f5fe1118b5f5fe111865f5fe111815f5fe1117c"
    "5f5fe111775f5fe111725f5fe1116d5f5fe111685f5fe111635f5fe1115e5f5fe111595f5fe111545f5fe1114f5f5f"
    "e1114a5f5fe111455f5fe111405f5fe1113b5f5fe111365f5fe111315f5fe1112c5f5fe111275f5fe111225f5fe111"
    "1d5f5fe111185f5fe111135f5fe1110e5f5fe111095f5fe111045f5fe110ff5f5fe110fa5f5fe110f55f5fe110f05f"
    "5fe110eb5f5fe110e65f5fe110e15f5fe110dc5f5fe110d75f5fe110d25f5fe110cd5f5fe110c85f5fe110c35f5fe1"
    "10be5f5fe110b95f5fe110b45f5fe110af5f5fe110aa5f5fe110a55f5fe110a05f5fe1109b5f5fe110965f5fe11091"
    "5f5fe1108c5f5fe110875f5fe110825f5fe1107d5f5fe110785f5fe110735f5fe1106e5f5fe110695f5fe110645f5f"
    "e1105f5f5fe1105a5f5fe110555f5fe110505f5fe1104b5f5fe110465f5fe110415f5fe1103c5f5fe110375f5fe110"
    "325f5fe1102d5f5fe110285f5fe110235f5fe1101e5f5fe110195f5fe110145f5fe1100f5f5fe1100a5f5fe110055f"
    "5fe110005f5fe10ffb5f5fe10ff65f5fe10ff15f5fe10fec5f5fe10fe75f5fe10fe25f5fe10fdd5f5fe10fd85f5fe1"
    "0fd35f5fe10fce5f5fe10fc95f5fe10fc45f5fe10fbf5f5fe10fba5f5fe10fb55f5fe10fb05f5fe10fab5f5fe10fa6"
    "5f5fe10fa15f5fe10f9c5f5fe10f975f5fe10f925f5fe10f8d5f5fe10f885f5fe10f835f5fe10f7e5f5fe10f795f5f"
    "e10f745f5fe10f6f5f5fe10f6a5f5fe10f655f5fe10f605f5fe10f5b5f5fe10f565f5fe10f515f5fe10f4c5f5fe10f"
    "475f5fe10f425f5fe10f3d5f5fe10f385f5fe10f335f5fe10f2e5f5fe10f295f5fe10f245f5fe10f1f5f5fe10f1a5f"
    "5fe10f155f5fe10f105f5fe10f0b5f5fe10f065f5fe10f015f5fe10efc5f5fe10ef75f5fe10ef25f5fe10eed5f5fe1"
    "0ee85f5fe10ee35f5fe10ede5f5fe10ed95f5fe10ed45f5fe10ecf5f5fe10eca5f5fe10ec55f5fe10ec05f5fe10ebb"
    "5f5fe10eb65f5fe10eb15f5fe10eac5f5fe10ea75f5fe10ea25f5fe10e9d5f5fe10e985f5fe10e935f5fe10e8e5f5f"
    "e10e895f5fe10e845f5fe10e7f5f5fe10e7a5f5fe10e755f5fe10e705f5fe10e6b5f5fe10e665f5fe10e615f5fe10e"
    "5c5f5fe10e575f5fe10e525f5fe10e4d5f5fe10e485f5fe10e435f5fe10e3e5f5fe10e395f5fe10e345f5fe10e2f5f"
    "5fe10e2a5f5fe10e255f5fe10e205f5fe10e1b5f5fe10e165f5fe10e115f5fe10e0c5f5fe10e075f5fe10e025f5fe1"
    "0dfd5f5fe10df85f5fe10df35f5fe10dee5f5fe10de95f5fe10de45f5fe10ddf5f5fe10dda5f5fe10dd55f5fe10dd0"
    "5f5fe10dcb5f5fe10dc65f5fe10dc15f5fe10dbc5f5fe10db75f5fe10db25f5fe10dad5f5fe10da85f5fe10da35f5f"
    "e10d9e5f5fe10d995f5fe10d945f5fe10d8f5f5fe10d8a5f5fe10d855f5fe10d805f5fe10d7b5f5fe10d765f5fe10d"
    "715f5fe10d6c5f5fe10d675f5fe10d625f5fe10d5d5f5fe10d585f5fe10d535f5fe10d4e5f5fe10d495f5fe10d445f"
    "5fe10d3f5f5fe10d3a5f5fe10d355f5fe10d305f5fe10d2b5f5fe10d265f5fe10d215f5fe10d1c5f5fe10d175f5fe1"
    "0d125f5fe10d0d5f5fe10d085f5fe10d035f5fe10cfe5f5fe10cf95f5fe10cf45f5fe10cef5f5fe10cea5f5fe10ce5"
    "5f5fe10ce05f5fe10cdb5f5fe10cd65f5fe10cd15f5fe10ccc5f5fe10cc75f5fe10cc25f5fe10cbd5f5fe10cb85f5f"
    "e10cb35f5fe10cae5f5fe10ca95f5fe10ca45f5fe10c9f5f5fe10c9a5f5fe10c955f5fe10c905f5fe10c8b5f5fe10c"
    "865f5fe10c815f5fe10c7c5f5fe10c775f5fe10c725f5fe10c6d5f5fe10c685f5fe10c635f5fe10c5e5f5fe10c595f"
    "5fe10c545f5fe10c4f5f5fe10c4a5f5fe10c455f5fe10c405f5fe10c3b5f5fe10c365f5fe10c315f5fe10c2c5f5fe1"
    "0c275f5fe10c225f5fe10c1d5f5fe10c185f5fe10c135f5fe10c0e5f5fe10c095f5fe10c045f5fe10bff5f5fe10bfa"
    "5f5fe10bf55f5fe10bf05f5fe10beb5f5fe10be65f5fe10be15f5fe10bdc5f5fe10bd75f5fe10bd25f5fe10bcd5f5f"
    "e10bc85f5fe10bc35f5fe10bbe5f5fe10bb95f5fe10bb45f5fe10baf5f5fe10baa5f5fe10ba55f5fe10ba05f5fe10b"
    "9b5f5fe10b965f5fe10b915f5fe10b8c5f5fe10b875f5fe10b825f5fe10b7d5f5fe10b785f5fe10b735f5fe10b6e5f"
    "5fe10b695f5fe10b645f5fe10b5f5f5fe10b5a5f5fe10b555f5fe10b505f5fe10b4b5f5fe10b465f5fe10b415f5fe1"
    "0b3c5f5fe10b375f5fe10b325f5fe10b2d5f5fe10b285f5fe10b235f5fe10b1e5f5fe10b195f5fe10b145f5fe10b0f"
    "5f5fe10b0a5f5fe10b055f5fe10b005f5fe10afb5f5fe10af65f5fe10af15f5fe10aec5f5fe10ae75f5fe10ae25f5f"
    "e10add5f5fe10ad85f5fe10ad35f5fe10ace5f5fe10ac95f5fe10ac45f5fe10abf5f5fe10aba5f5fe10ab55f5fe10a"
    "b05f5fe10aab5f5fe10aa65f5fe10aa15f5fe10a9c5f5fe10a975f5fe10a925f5fe10a8d5f5fe10a885f5fe10a835f"
    "5fe10a7e5f5fe10a795f5fe10a745f5fe10a6f5f5fe10a6a5f5fe10a655f5fe10a605f5fe10a5b5f5fe10a565f5fe1"
    "0a515f5fe10a4c5f5fe10a475f5fe10a425f5fe10a3d5f5fe10a385f5fe10a335f5fe10a2e5f5fe10a295f5fe10a24"
    "5f5fe10a1f5f5fe10a1a5f5fe10a155f5fe10a105f5fe10a0b5f5fe10a065f5fe10a015f5fe109fc5f5fe109f75f5f"
    "e109f25f5fe109ed5f5fe109e85f5fe109e35f5fe109de5f5fe109d95f5fe109d45f5fe109cf5f5fe109ca5f5fe109"
    "c55f5fe109c05f5fe109bb5f5fe109b65f5fe109b15f5fe109ac5f5fe109a75f5fe109a25f5fe1099d5f5fe109985f"
    "5fe109935f5fe1098e5f5fe109895f5fe109845f5fe1097f5f5fe1097a5f5fe109755f5fe109705f5fe1096b5f5fe1"
    "09665f5fe109615f5fe1095c5f5fe109575f5fe109525f5fe1094d5f5fe109485f5fe109435f5fe1093e5f5fe10939"
    "5f5fe109345f5fe1092f5f5fe1092a5f5fe109255f5fe109205f5fe1091b5f5fe109165f5fe109115f5fe1090c5f5f"
    "e109075f5fe109025f5fe108fd5f5fe108f85f5fe108f35f5fe108ee5f5fe108e95f5fe108e45f5fe108df5f5fe108"
    "da5f5fe108d55f5fe108d05f5fe108cb5f5fe108c65f5fe108c15f5fe108bc5f5fe108b75f5fe108b25f5fe108ad5f"
    "5fe108a85f5fe108a35f5fe1089e5f5fe108995f5fe108945f5fe1088f5f5fe1088a5f5fe108855f5fe108805f5fe1"
    "087b5f5fe108765f5fe108715f5fe1086c5f5fe108675f5fe108625f5fe1085d5f5fe108585f5fe108535f5fe1084e"
    "5f5fe108495f5fe108445f5fe1083f5f5fe1083a5f5fe108355f5fe108305f5fe1082b5f5fe108265f5fe108215f5f"
    "e1081c5f5fe108175f5fe108125f5fe1080d5f5fe108085f5fe108035f5fe107fe5f5fe107f95f5fe107f45f5fe107"
    "ef5f5fe107ea5f5fe107e55f5fe107e05f5fe107db5f5fe107d65f5fe107d15f5fe107cc5f5fe107c75f5fe107c25f"
    "5fe107bd5f5fe107b85f5fe107b35f5fe107ae5f5fe107a95f5fe107a45f5fe1079f5f5fe1079a5f5fe107955f5fe1"
    "07905f5fe1078b5f5fe107865f5fe107815f5fe1077c5f5fe107775f5fe107725f5fe1076d5f5fe107685f5fe10763"
    "5f5fe1075e5f5fe107595f5fe107545f5fe1074f5f5fe1074a5f5fe107455f5fe107405f5fe1073b5f5fe107365f5f"
    "e107315f5fe1072c5f5fe107275f5fe107225f5fe1071d5f5fe107185f5fe107135f5fe1070e5f5fe107095f5fe107"
    "045f5fe106ff5f5fe106fa5f5fe106f55f5fe106f05f5fe106eb5f5fe106e65f5fe106e15f5fe106dc5f5fe106d75f"
    "5fe106d25f5fe106cd5f5fe106c85f5fe106c35f5fe106be5f5fe106b95f5fe106b45f5fe106af5f5fe106aa5f5fe1"
    "06a55f5fe106a05f5fe1069b5f5fe106965f5fe106915f5fe1068c5f5fe106875f5fe106825f5fe1067d5f5fe10678"
    "5f5fe106735f5fe1066e5f5fe106695f5fe106645f5fe1065f5f5fe1065a5f5fe106555f5fe106505f5fe1064b5f5f"
    "e106465f5fe106415f5fe1063c5f5fe106375f5fe106325f5fe1062d5f5fe106285f5fe106235f5fe1061e5f5fe106"
    "195f5fe106145f5fe1060f5f5fe1060a5f5fe106055f5fe106005f5fe105fb5f5fe105f65f5fe105f15f5fe105ec5f"
    "5fe105e75f5fe105e25f5fe105dd5f5fe105d85f5fe105d35f5fe105ce5f5fe105c95f5fe105c45f5fe105bf5f5fe1"
    "05ba5f5fe105b55f5fe105b05f5fe105ab5f5fe105a65f5fe105a15f5fe1059c5f5fe105975f5fe105925f5fe1058d"
    "5f5fe105885f5fe105835f5fe1057e5f5fe105795f5fe105745f5fe1056f5f5fe1056a5f5fe105655f5fe105605f5f"
    "e1055b5f5fe105565f5fe105515f5fe1054c5f5fe105475f5fe105425f5fe1053d5f5fe105385f5fe105335f5fe105"
    "2e5f5fe105295f5fe105245f5fe1051f5f5fe1051a5f5fe105155f5fe105105f5fe1050b5f5fe105065f5fe105015f"
    "5fe104fc5f5fe104f75f5fe104f25f5fe104ed5f5fe104e85f5fe104e35f5fe104de5f5fe104d95f5fe104d45f5fe1"
    "04cf5f5fe104ca5f5fe104c55f5fe104c05f5fe104bb5f5fe104b65f5fe104b15f5fe104ac5f5fe104a75f5fe104a2"
    "5f5fe1049d5f5fe104985f5fe104935f5fe1048e5f5fe104895f5fe104845f5fe1047f5f5fe1047a5f5fe104755f5f"
    "e104705f5fe1046b5f5fe104665f5fe104615f5fe1045c5f5fe104575f5fe104525f5fe1044d5f5fe104485f5fe104"
    "435f5fe1043e5f5fe104395f5fe104345f5fe1042f5f5fe1042a5f5fe104255f5fe104205f5fe1041b5f5fe104165f"
    "5fe104115f5fe1040c5f5fe104075f5fe104025f5fe103fd5f5fe103f85f5fe103f35f5fe103ee5f5fe103e95f5fe1"
    "03e45f5fe103df5f5fe103da5f5fe103d55f5fe103d05f5fe103cb5f5fe103c65f5fe103c15f5fe103bc5f5fe103b7"
    "5f5fe103b25f5fe103ad5f5fe103a85f5fe103a35f5fe1039e5f5fe103995f5fe103945f5fe1038f5f5fe1038a5f5f"
    "e103855f5fe103805f5fe1037b5f5fe103765f5fe103715f5fe1036c5f5fe103675f5fe103625f5fe1035d5f5fe103"
    "585f5fe103535f5fe1034e5f5fe103495f5fe103445f5fe1033f5f5fe1033a5f5fe103355f5fe103305f5fe1032b5f"
    "5fe103265f5fe103215f5fe1031c5f5fe103175f5fe103125f5fe1030d5f5fe103085f5fe103035f5fe102fe5f5fe1"
    "02f95f5fe102f45f5fe102ef5f5fe102ea5f5fe102e55f5fe102e05f5fe102db5f5fe102d65f5fe102d15f5fe102cc"
    "5f5fe102c75f5fe102c25f5fe102bd5f5fe102b85f5fe102b35f5fe102ae5f5fe102a95f5fe102a45f5fe1029f5f5f"
    "e1029a5f5fe102955f5fe102905f5fe1028b5f5fe102865f5fe102815f5fe1027c5f5fe102775f5fe102725f5fe102"
    "6d5f5fe102685f5fe102635f5fe1025e5f5fe102595f5fe102545f5fe1024f5f5fe1024a5f5fe102455f5fe102405f"
    "5fe1023b5f5fe102365f5fe102315f5fe1022c5f5fe102275f5fe102225f5fe1021d5f5fe102185f5fe102135f5fe1"
    "020e5f5fe102095f5fe102045f5fe101ff5f5fe101fa5f5fe101f55f5fe101f05f5fe101eb5f5fe101e65f5fe101e1"
    "5f5fe101dc5f5fe101d75f5fe101d25f5fe101cd5f5fe101c85f5fe101c35f5fe101be5f5fe101b95f5fe101b45f5f"
    "e101af5f5fe101aa5f5fe101a55f5fe101a05f5fe1019b5f5fe101965f5fe101915f5fe1018c5f5fe101875f5fe101"
    "825f5fe1017d5f5fe101785f5fe101735f5fe1016e5f5fe101695f5fe101645f5fe1015f5f5fe1015a5f5fe101555f"
    "5fe101505f5fe1014b5f5fe101465f5fe101415f5fe1013c5f5fe101375f5fe101325f5fe1012d5f5fe101285f5fe1"
    "01235f5fe1011e5f5fe101195f5fe101145f5fe1010f5f5fe1010a5f5fe101055f5fe101005f5fe100fb5f5fe100f6"
    "5f5fe100f15f5fe100ec5f5fe100e75f5fe100e25f5fe100dd5f5fe100d85f5fe100d35f5fe100ce5f5fe100c95f5f"
    "e100c45f5fe100bf5f5fe100ba5f5fe100b55f5fe100b05f5fe100ab5f5fe100a65f5fe100a15f5fe1009c5f5fe100"
    "975f5fe100925f5fe1008d5f5fe100885f5fe100835f5fe1007e5f5fe100795f5fe100745f5fe1006f5f5fe1006a5f"
    "5fe100655f5fe100605f5fe1005b5f5fe100565f5fe100515f5fe1004c5f5fe100475f5fe100425f5fe1003d5f5fe1"
    "00385f5fe100335f5fe1002e5f5fe100295f5fe100245f5fe1001f5f5fe1001a5f5fe100155f5fe100105f5fe1000b"
    "5f5fe100065f5fe100015f00")
                                        .value();

void eof_validation(benchmark::State& state, evmone::ContainerKind kind, const bytes& container)
{
    for (auto _ : state)
    {
        const auto res = evmone::validate_eof(EVMC_OSAKA, kind, container);
        if (res != evmone::EOFValidationError::success)
            state.SkipWithError(evmone::get_error_message(res).data());
    }

    using namespace benchmark;
    const auto total_size =
        static_cast<double>(state.iterations() * static_cast<IterationCount>(container.size()));
    state.counters["size"] = {static_cast<double>(container.size()), {}, Counter::kIs1024};
    state.counters["bytes_rate"] = {total_size, Counter::kIsRate, Counter::kIs1024};
    state.counters["gas_rate"] = {total_size / 16, Counter::kIsRate};
}

using enum evmone::ContainerKind;
BENCHMARK_CAPTURE(eof_validation, max_code_sections, runtime, max_code_sections)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK_CAPTURE(eof_validation, stack_height_max_span, runtime, stack_height_max_span)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK_CAPTURE(eof_validation, max_nested_containers, runtime, max_nested_containers)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK_CAPTURE(eof_validation, max_nested_containers2, runtime, max_nested_containers2)
    ->Unit(benchmark::kMicrosecond);
}  // namespace
