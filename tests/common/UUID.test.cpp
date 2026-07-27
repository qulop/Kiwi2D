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
    EXPECT_EQ(uuid_0, uuid_1.GetValue());
}

TEST(UUID_Test, AssociativeContainers) {
    using namespace Kiwi;

    const UUID uuid_0 = UUID::Generate();
    const UUID uuid_1 = UUID::Generate();

    std::unordered_map<UUID, String> hashMap;
    hashMap.emplace(uuid_0, "AMD");
    hashMap.emplace(uuid_1, "NVIDIA");

    EXPECT_EQ(hashMap.at(uuid_0), "AMD");
    EXPECT_EQ(hashMap.at(uuid_1), "NVIDIA");


    std::map<UUID, String> map;
    map.emplace(uuid_0, "AMD");
    map.emplace(uuid_1, "NVIDIA");

    EXPECT_EQ(map.at(uuid_0), "AMD");
    EXPECT_EQ(map.at(uuid_1), "NVIDIA");
}
