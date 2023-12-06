#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "Types.h"
#include "Log.cpp"

TEST_CASE("Test Types", "[Types]")
{
    REQUIRE(typeid(f64) == typeid(double));
    REQUIRE(typeid(f32) == typeid(float));
    REQUIRE(typeid(i64) == typeid(int64_t));
    REQUIRE(typeid(i32) == typeid(int32_t));
    REQUIRE(typeid(i16) == typeid(int16_t));
    REQUIRE(typeid(i8) == typeid(int8_t));
    REQUIRE(typeid(u64) == typeid(uint64_t));
    REQUIRE(typeid(u32) == typeid(uint32_t));
    REQUIRE(typeid(u16) == typeid(uint16_t));
    REQUIRE(typeid(u8) == typeid(uint8_t));

    Unique<i32> uniqueI32;
    REQUIRE(uniqueI32 == nullptr);
    uniqueI32 = MakeUnique<i32>(6);
    REQUIRE(uniqueI32 != nullptr);
    REQUIRE(*uniqueI32 == 6);

    Shared<i32> sharedI32;
    REQUIRE(sharedI32 == nullptr);
    sharedI32 = MakeShared<i32>(6);
    REQUIRE(uniqueI32 != nullptr);
    REQUIRE(*uniqueI32 == 6);

    Shared<i32> newShared = sharedI32;
    REQUIRE(newShared.use_count() == 2);
}

//TODO: Make not visual somehow
TEST_CASE("Test Logging (Visual)", "[Log]")
{
    Log::Init();
    // Test levels
    SK_CORE_TRACE("Hello");
    SK_CORE_INFO("Hello");
    SK_CORE_WARN("Hello");
    SK_CORE_ERROR("Hello");
    SK_CORE_CRITICAL("Hello");

    // Test formatting
    SK_CORE_TRACE("Hello, %d %c %s %f", 1, 'a', "string", 5.5);
    // Test Bad Format
    //TODO: check for bad formatting
    SK_CORE_TRACE("Hello, %s");
    SK_CORE_TRACE("Hello", 1);

    Log::Shutdown();
}

TEST_CASE("Test Event", "[Event]")
{
    //TODO:
}

TEST_CASE("Test Application", "[Application]")
{
    //TODO:
}

TEST_CASE("Test ApplicationLayer", "[ApplicationLayer]")
{
    //TODO:
}
