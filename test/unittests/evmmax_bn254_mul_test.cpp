// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2023 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "../utils/utils.hpp"
#include "evmone_precompiles/bn254.hpp"
#include "test/precompiles_evm/bn254_evm.hpp"
#include <gtest/gtest.h>

using namespace evmmax::bn254;
using namespace intx;

namespace
{
struct TestCase
{
    bytes input;
    bytes expected_output;

    TestCase(bytes i, bytes o) : input{std::move(i)}, expected_output{std::move(o)}
    {
        input.resize(96);
        expected_output.resize(64);
    }
};

const TestCase
    test_cases
        [] =
            {
                {"025a6f4181d2b4ea8b724290ffb40156eb0adb514c688556eb79cdea0752c2bb2eff3f31dea215f1eb86023a133a996eb6300b44da664d64251d05381bb8a02e183227397098d014dc2822db40c0ac2ecbc0b548b438e5469e10460b6c3e7ea3"_hex,
                    "14789d0d4a730b354403b5fac948113739e276c23e0258d8596ee72f9cd9d3230af18a63153e0ec25ff9f2951dd3fa90ed0197bfef6e2a1a62b5095b9d2b4a27"_hex},  // from https://github.com/sedaprotocol/bn254/blob/main/src/bn256.json#L68
                {"070a8d6a982153cae4be29d434e8faef8a47b274a053f5a4ee2a6c9c13c31e5c031b8ce914eba3a9ffb989f9cdd5b0f01943074bf4f0f315690ec3cec6981afc30644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd46"_hex,
                    "025a6f4181d2b4ea8b724290ffb40156eb0adb514c688556eb79cdea0752c2bb2eff3f31dea215f1eb86023a133a996eb6300b44da664d64251d05381bb8a02e"_hex},  // https://github.com/sedaprotocol/bn254/blob/main/src/bn256.json#LL62C1-L62C1
                {"0f25929bcb43d5a57391564615c9e70a992b10eafa4db109709649cf48c50dd216da2f5cb6be7a0aa72c440c53c9bbdfec6c36c7d515536431b3a865468acbba0000000000000000000000000000000000000000000000000000000000000003"_hex,
                    "1f4d1d80177b1377743d1901f70d7389be7f7a35a35bfd234a8aaee615b88c49018683193ae021a2f8920fed186cde5d9b1365116865281ccf884c1f28b1df8f"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "030644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd315ed738c0e0a7c92e7845f96b2ae9c0a68a6a449e3538fc7ff3ebf7a5a18a2c4"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000002"_hex,
                    "030644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd315ed738c0e0a7c92e7845f96b2ae9c0a68a6a449e3538fc7ff3ebf7a5a18a2c4"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "13b8fec4a1eb2c7e3ccc07061ad516277c3bbe57bd4a302012b58a517f6437a4224d978b5763831dff16ce9b2c42222684835fedfc70ffec005789bb0c10de36"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000001"_hex,
                    "13b8fec4a1eb2c7e3ccc07061ad516277c3bbe57bd4a302012b58a517f6437a4224d978b5763831dff16ce9b2c42222684835fedfc70ffec005789bb0c10de36"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000100000000000000000000000000000000"_hex,
                    "13b8fec4a1eb2c7e3ccc07061ad516277c3bbe57bd4a302012b58a517f6437a4224d978b5763831dff16ce9b2c42222684835fedfc70ffec005789bb0c10de36"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000230644e72e131a029b85045b68181585d2833e84879b9709143e1f593f00000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd45"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000230644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000000"_hex,
                    "000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd45"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000230644e72e131a029b85045b68181585d2833e84879b9709143e1f593f00000010000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000230644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000001"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"00000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000002ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "2f588cffe99db877a4434b598ab28f81e0522910ea52b45f0adaa772b2d5d35212f42fa8fd34fb1b33d8c6a718b6590198389b26fc9d8808d971f8b009777a97"_hex},
                {"00000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000002ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"_hex,
                    "2f588cffe99db877a4434b598ab28f81e0522910ea52b45f0adaa772b2d5d35212f42fa8fd34fb1b33d8c6a718b6590198389b26fc9d8808d971f8b009777a97"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000090000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "039730ea8dff1254c0fee9c0ea777d29a9c710b7e616683f194f18c43b43b869073a5ffcc6fc7a28c30723d6e58ce577356982d65b833a5a5c15bf9024b43d98"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000009"_hex,
                    "039730ea8dff1254c0fee9c0ea777d29a9c710b7e616683f194f18c43b43b869073a5ffcc6fc7a28c30723d6e58ce577356982d65b833a5a5c15bf9024b43d98"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f6"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f600000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f60000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f600000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "1051acb0700ec6d42a88215852d582efbaef31529b6fcbc3277b5c1b300f5cf0135b2394bb45ab04b8bd7611bd2dfe1de6a4e6e2ccea1ea1955f577cd66af85b"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f600000000000000000000000000000001"_hex,
                    "1051acb0700ec6d42a88215852d582efbaef31529b6fcbc3277b5c1b300f5cf0135b2394bb45ab04b8bd7611bd2dfe1de6a4e6e2ccea1ea1955f577cd66af85b"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f60000000000000000000000000000000100000000000000000000000000000000"_hex,
                    "1051acb0700ec6d42a88215852d582efbaef31529b6fcbc3277b5c1b300f5cf0135b2394bb45ab04b8bd7611bd2dfe1de6a4e6e2ccea1ea1955f577cd66af85b"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f600000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f6"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f60000000000000000000000000000000000000000000000000000000000000001"_hex,
                    "1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f6"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f600000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "03d64e49ebb3c56c99e0769c1833879c9b86ead23945e1e7477cbd057e961c500d6840b39f8c2fefe0eced3e7d210b830f50831e756f1cc9039af65dc292e6d0"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f60000000000000000000000000000000000000000000000000000000000000002"_hex,
                    "03d64e49ebb3c56c99e0769c1833879c9b86ead23945e1e7477cbd057e961c500d6840b39f8c2fefe0eced3e7d210b830f50831e756f1cc9039af65dc292e6d0"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f630644e72e131a029b85045b68181585d2833e84879b9709143e1f593f00000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe3163511ddc1c3f25d396745388200081287b3fd1472d8339d5fecb2eae0830451"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f630644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000000"_hex,
                    "1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe3163511ddc1c3f25d396745388200081287b3fd1472d8339d5fecb2eae0830451"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f630644e72e131a029b85045b68181585d2833e84879b9709143e1f593f00000010000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f630644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000001"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f6ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "2cde5879ba6f13c0b5aa4ef627f159a3347df9722efce88a9afbb20b763b4c411aa7e43076f6aee272755a7f9b84832e71559ba0d2e0b17d5f9f01755e5b0d11"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f6ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"_hex,
                    "2cde5879ba6f13c0b5aa4ef627f159a3347df9722efce88a9afbb20b763b4c411aa7e43076f6aee272755a7f9b84832e71559ba0d2e0b17d5f9f01755e5b0d11"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f600000000000000000000000000000000000000000000000000000000000000090000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "1dbad7d39dbc56379f78fac1bca147dc8e66de1b9d183c7b167351bfe0aeab742cd757d51289cd8dbd0acf9e673ad67d0f0a89f912af47ed1be53664f5692575"_hex},
                {"1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe31a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f60000000000000000000000000000000000000000000000000000000000000009"_hex,
                    "1dbad7d39dbc56379f78fac1bca147dc8e66de1b9d183c7b167351bfe0aeab742cd757d51289cd8dbd0acf9e673ad67d0f0a89f912af47ed1be53664f5692575"_hex},
                {"0f25929bcb43d5a57391564615c9e70a992b10eafa4db109709649cf48c50dd216da2f5cb6be7a0aa72c440c53c9bbdfec6c36c7d515536431b3a865468acbba0000000000000000000000000000000000000000000000000000000000000002"_hex,
                    "1de49a4b0233273bba8146af82042d004f2085ec982397db0d97da17204cc2860217327ffc463919bef80cc166d09c6172639d8589799928761bcd9f22c903d4"_hex},
                {"1f4d1d80177b1377743d1901f70d7389be7f7a35a35bfd234a8aaee615b88c49018683193ae021a2f8920fed186cde5d9b1365116865281ccf884c1f28b1df8f0000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"1f4d1d80177b1377743d1901f70d7389be7f7a35a35bfd234a8aaee615b88c492eddcb59a6517e86bfbe35c9691479fffc6e0580000ca2706c983ff7afcb1db80000000000000000000000000000000000000000000000000000000000000002"_hex,
                    "255e468453d7636cc1563e43f7521755f95e6c56043c7321b4ae04e772945fb00225c5f1623620fd84bfbab2d861a9d1e570f7727c540f403085998ebaf407c4"_hex},
                {"1f4d1d80177b1377743d1901f70d7389be7f7a35a35bfd234a8aaee615b88c49018683193ae021a2f8920fed186cde5d9b1365116865281ccf884c1f28b1df8f30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000000"_hex,
                    "1f4d1d80177b1377743d1901f70d7389be7f7a35a35bfd234a8aaee615b88c492eddcb59a6517e86bfbe35c9691479fffc6e0580000ca2706c983ff7afcb1db8"_hex},
                {"1f4d1d80177b1377743d1901f70d7389be7f7a35a35bfd234a8aaee615b88c49018683193ae021a2f8920fed186cde5d9b1365116865281ccf884c1f28b1df8f30644e72e131a029b85045b68181585d2833e84879b9709143e1f593efffffff"_hex,
                    "255e468453d7636cc1563e43f7521755f95e6c56043c7321b4ae04e772945fb00225c5f1623620fd84bfbab2d861a9d1e570f7727c540f403085998ebaf407c4"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030644e72e131a029b85045b68181585d2833e84879b9709143e1f593efffffff"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd46"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd45"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd450000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd450000000000000000000000000000000000000000000000000000000000000002"_hex,
                    "030644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd31a76dae6d3272396d0cbe61fced2bc532edac647851e3ac53ce1cc9c7e645a83"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd450000000000000000000000000000000000000000000000000000000000000003"_hex,
                    "0769bf9ac56bea3ff40232bcb1b6bd159315d84715b8e679f2d355961915abf005acb4b400e90c0063006a39f478f3e865e306dd5cd56f356e2e8cd8fe7edae6"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd4530644e72e131a029b85045b68181585d2833e84879b9709143e1f593efffffff"_hex,
                    "030644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd315ed738c0e0a7c92e7845f96b2ae9c0a68a6a449e3538fc7ff3ebf7a5a18a2c4"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd4530644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000002"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd450000000000000000000000000000000000000000000000000000000000000001"_hex,
                    "000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd45"_hex},
                {"0ccbec17235f5b9cc5e42f3df6364a76ecdd0101ddda8fc5dc0ba0b59c0e5628069ef5e376c0a1ea82f9dfc2e0001a7f385d655eef9a6f976c7a5d2c493ea3ad0000000000000000000000000000000000000000000000000000000000000002"_hex,
                    "1fd3b816d9951dcb9aa9797d25e51a865987703ae83cd69c4658679f0350ae2b29ce3d80a74ddc13784beb25ca9fbfd048a3265a32c6f38b92060c5093a0e7a7"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd4530644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd46"_hex,
                    "0ccbec17235f5b9cc5e42f3df6364a76ecdd0101ddda8fc5dc0ba0b59c0e5628069ef5e376c0a1ea82f9dfc2e0001a7f385d655eef9a6f976c7a5d2c493ea3ad"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd4530644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd45"_hex,
                    "2c15ed1902e189486ab6b625aa982510aef6246b21a1e1bcea382da4d735e8ba02103e58cbd2fa8081763442ab46c26a9b8051e9b049c3948c8d7d0e139c5e3f"_hex},
                {"2f588cffe99db877a4434b598ab28f81e0522910ea52b45f0adaa772b2d5d3521d701ec9e3fca50e84777f0f68caff5bff48cf6a6bd4428462ae9366cf0582b00000000000000000000000000000000000000000000000000000000000000002"_hex,
                    "2fa739d4cde056d8fd75427345cbb34159856e06a4ffad64159c4773f23fbf4b1eed5d5325c31fc89dd541a13d7f63b981fae8d4bf78a6b08a38a601fcfea97b"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd45ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"_hex,
                    "2f588cffe99db877a4434b598ab28f81e0522910ea52b45f0adaa772b2d5d3521d701ec9e3fca50e84777f0f68caff5bff48cf6a6bd4428462ae9366cf0582b0"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000130644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd45fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe"_hex,
                    "08e2142845db159bd105879a109fe7a6f254ed3ddae0e9cd8a2aeae05e5f647b221108ee615499d2e0a1113ca1a858a34e055f9da2d30e6e6ab392b049944a92"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000003"_hex,
                    "0769bf9ac56bea3ff40232bcb1b6bd159315d84715b8e679f2d355961915abf02ab799bee0489429554fdb7c8d086475319e63b40b9c5b57cdf1ff3dd9fe2261"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000230644e72e131a029b85045b68181585d2833e84879b9709143e1f593efffffff"_hex,
                    "030644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd31a76dae6d3272396d0cbe61fced2bc532edac647851e3ac53ce1cc9c7e645a83"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000001"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000002"_hex},
                {"0ccbec17235f5b9cc5e42f3df6364a76ecdd0101ddda8fc5dc0ba0b59c0e562829c5588f6a70fe3f355665f3a1813dde5f24053278d75af5cfa62eea8f3e599a0000000000000000000000000000000000000000000000000000000000000002"_hex,
                    "1fd3b816d9951dcb9aa9797d25e51a865987703ae83cd69c4658679f0350ae2b069610f239e3c41640045a90b6e1988d4ede443735aad701aa1a7fc644dc15a0"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000230644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd46"_hex,
                    "0ccbec17235f5b9cc5e42f3df6364a76ecdd0101ddda8fc5dc0ba0b59c0e562829c5588f6a70fe3f355665f3a1813dde5f24053278d75af5cfa62eea8f3e599a"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000230644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd45"_hex,
                    "2c15ed1902e189486ab6b625aa982510aef6246b21a1e1bcea382da4d735e8ba2e54101a155ea5a936da1173d63a95f2fc0118a7b82806f8af930f08c4e09f08"_hex},
                {"2f588cffe99db877a4434b598ab28f81e0522910ea52b45f0adaa772b2d5d35212f42fa8fd34fb1b33d8c6a718b6590198389b26fc9d8808d971f8b009777a970000000000000000000000000000000000000000000000000000000000000002"_hex,
                    "2fa739d4cde056d8fd75427345cbb34159856e06a4ffad64159c4773f23fbf4b1176f11fbb6e80611a7b04154401f4a4158681bca8f923dcb1e7e614db7e53cc"_hex},
                {"00000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000002fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe"_hex,
                    "08e2142845db159bd105879a109fe7a6f254ed3ddae0e9cd8a2aeae05e5f647b0e5345847fdd0656d7af3479dfd8ffba497c0af3c59ebc1ed16cf9668ee8b2b5"_hex},
                {"00000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030644e72e131a029b85045b68181585d2833e84879b9709143e1f593f00000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030644e72e131a029b85045b68181585d2833e84879b9709143e1f593f00000010000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000001"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000090000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000009"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"00000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000002"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_hex},
                {"0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000"_hex,
                    "00000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000002"_hex},
};

}  // namespace

TEST(evmmax, bn254_mul_validate_inputs)
{
    const evmmax::ModArith s{evmmax::bn254::FieldPrime};

    for (const auto& t : test_cases)
    {
        ASSERT_EQ(t.input.size(), 96);
        ASSERT_EQ(t.expected_output.size(), 64);

        const Point a{
            be::unsafe::load<uint256>(t.input.data()), be::unsafe::load<uint256>(&t.input[32])};
        const Point e{be::unsafe::load<uint256>(t.expected_output.data()),
            be::unsafe::load<uint256>(&t.expected_output[32])};

        EXPECT_TRUE(validate(a));
        EXPECT_TRUE(validate(e));
    }
}

TEST(evmmax, bn254_pt_mul)
{
    const evmmax::ModArith s{evmmax::bn254::FieldPrime};

    for (const auto& t : test_cases)
    {
        const Point p{
            be::unsafe::load<uint256>(t.input.data()), be::unsafe::load<uint256>(&t.input[32])};
        const auto d{be::unsafe::load<uint256>(&t.input[64])};
        const Point e{be::unsafe::load<uint256>(t.expected_output.data()),
            be::unsafe::load<uint256>(&t.expected_output[32])};

        EXPECT_EQ(mul(p, d), e);
        EXPECT_EQ(evmmax::evm::bn254::mul(p, d), e);
    }
}
