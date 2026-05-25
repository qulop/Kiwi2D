#include <gtest/gtest.h>

#include <common/UUID.hpp>
#include <common/types/String.hpp>


TEST(UUID_Test, InitializationWithKnownUUID) {
    using namespace Kiwi;

    constexpr StringView KNOWN_UUID_STR_HEX = "f47ac10b-58cc-4372-a567-0e02b2c3d479";
    constexpr StringView KNOWN_UUID_STR = "324969006592305634633390616021200786553";
    constexpr UInt128 KNOWN_UUID = KNOWN_UUID_STR.data();

    constexpr UUID uuid(KNOWN_UUID);

    EXPECT_TRUE(uuid == KNOWN_UUID);
    EXPECT_EQ(uuid.ToString(), KNOWN_UUID_STR_HEX);
}

TEST(UUID_Test, ReInitializationWithGeneratedUUID) {
    using namespace Kiwi;

    const UUID uuid_0 = UUID::Generate();
    const Opt<UUID> uuid_1 = UUID::FromString(uuid_0.ToString());

    EXPECT_TRUE(uuid_1);
    EXPECT_EQ(uuid_0, uuid_1.value());
}
