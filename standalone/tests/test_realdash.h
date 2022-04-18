#ifndef __R51_TESTS_TEST_REALDASH__
#define __R51_TESTS_TEST_REALDASH__

#include <Arduino.h>
#include <AUnit.h>
#include <Canny.h>

#include "mock_yield.h"
#include "mock_stream.h"
#include "src/realdash.h"
#include "testing.h"

using namespace aunit;
using ::Canny::Frame;


class FakeRealDash : public RealDash {
    public:
        bool readFilter(const Canny::Frame&) const override { return true; }
        bool writeFilter(const Canny::Frame&) const override { return true; }
};

test(RealDashTest, Read44) {
    MockYield yield(1);
    Frame expect(0x5800, 0, {0xf4, 0x08, 0x0e, 0xef, 0x39, 0x2c, 0x1b, 0x4c});
    byte buffer[] = {
        0x44, 0x33, 0x22, 0x11,
        0x00, 0x58, 0x00, 0x00,
        0xf4, 0x08, 0x0e, 0xef,
        0x39, 0x2c, 0x1b, 0x4c,
        0xc7,
    };

    FakeReadStream stream;
    stream.set(buffer, sizeof(buffer)/sizeof(buffer[0]));

    FakeRealDash realdash;
    realdash.begin(&stream);

    realdash.emit(yield.impl);
    assertEqual(yield.count(), 1);
    assertTrue(checkFrameEquals(yield.frames()[0], expect));
}

test(RealDashTest, Read66Short) {
    MockYield yield(1);
    Frame expect(0x5800, 0, {0xf4, 0x08, 0x0e, 0xef, 0x39, 0x2c, 0x1b, 0x4c});
    byte buffer[] = {
        0x66, 0x33, 0x22, 0x11,
        0x00, 0x58, 0x00, 0x00,
        0xf4, 0x08, 0x0e, 0xef,
        0x39, 0x2c, 0x1b, 0x4c,
        0xf2, 0x30, 0x3f, 0x6e,
    };

    FakeReadStream stream;
    stream.set(buffer, sizeof(buffer)/sizeof(buffer[0]));

    FakeRealDash realdash;
    realdash.begin(&stream);

    realdash.emit(yield.impl);
    assertEqual(yield.count(), 1);
    assertTrue(checkFrameEquals(yield.frames()[0], expect));
}

test(RealDashTest, Read66Long) {
    MockYield yield(1);
    Frame expect = Frame(0x5200, 0, {
        0xe9, 0x1c, 0xfe, 0x5a, 0xa1, 0x7a, 0x18, 0x4e,
        0xe1, 0x52, 0xff, 0x9a, 0x47, 0xe8, 0x27, 0x11,
        0x4f, 0xf4, 0x46, 0xf1, 0x5f, 0xca, 0xdd, 0x13,
        0x3f, 0x76, 0x27, 0x67, 0x03, 0xa9, 0x55, 0x1c,
        0x9b, 0x8e, 0x83, 0xe9, 0xff, 0x74, 0xde, 0x52,
        0x44, 0xa7, 0x75, 0x64, 0x86, 0x3d, 0xcc, 0x2a,
        0x91, 0x28, 0x28, 0xbe, 0x79, 0xf6, 0xaf, 0x50,
        0xfa, 0x6d, 0x31, 0xea, 0x9d, 0x8b, 0x5b, 0xd2,
    });
    byte buffer[] = {
        0x66, 0x33, 0x22, 0x1f, 0x00, 0x52, 0x00, 0x00,
        0xe9, 0x1c, 0xfe, 0x5a, 0xa1, 0x7a, 0x18, 0x4e,
        0xe1, 0x52, 0xff, 0x9a, 0x47, 0xe8, 0x27, 0x11,
        0x4f, 0xf4, 0x46, 0xf1, 0x5f, 0xca, 0xdd, 0x13,
        0x3f, 0x76, 0x27, 0x67, 0x03, 0xa9, 0x55, 0x1c,
        0x9b, 0x8e, 0x83, 0xe9, 0xff, 0x74, 0xde, 0x52,
        0x44, 0xa7, 0x75, 0x64, 0x86, 0x3d, 0xcc, 0x2a,
        0x91, 0x28, 0x28, 0xbe, 0x79, 0xf6, 0xaf, 0x50,
        0xfa, 0x6d, 0x31, 0xea, 0x9d, 0x8b, 0x5b, 0xd2,
        0x84, 0x98, 0x39, 0x0d,
    };

    FakeReadStream stream;
    stream.set(buffer, sizeof(buffer)/sizeof(buffer[0]));

    FakeRealDash realdash;
    realdash.begin(&stream);

    realdash.emit(yield.impl);
    assertEqual(yield.count(), 1);
    assertTrue(checkFrameEquals(yield.frames()[0], expect));
}

test(RealDashTest, ReadMulti) {
    MockYield yield(2);
    Frame expect1(0x5800, 0, {0xf4, 0x08, 0x0e, 0xef, 0x39, 0x2c, 0x1b, 0x4c});
    Frame expect2(0x5800, 0, {0xf4, 0x08, 0x0e, 0xef, 0x39, 0x2c, 0x1b, 0x4c});

    byte buffer[] = {
        // 0x44 frame
        0x44, 0x33, 0x22, 0x11,
        0x00, 0x58, 0x00, 0x00,
        0xf4, 0x08, 0x0e, 0xef,
        0x39, 0x2c, 0x1b, 0x4c,
        0xc7,
        // 0x66 frame
        0x66, 0x33, 0x22, 0x11,
        0x00, 0x58, 0x00, 0x00,
        0xf4, 0x08, 0x0e, 0xef,
        0x39, 0x2c, 0x1b, 0x4c,
        0xf2, 0x30, 0x3f, 0x6e,
    };

    FakeReadStream stream;
    stream.set(buffer, sizeof(buffer)/sizeof(buffer[0]));

    FakeRealDash realdash;
    realdash.begin(&stream);

    realdash.emit(yield.impl);
    realdash.emit(yield.impl);
    assertEqual(yield.count(), 2);
    assertTrue(checkFrameEquals(yield.frames()[0], expect1));
    assertTrue(checkFrameEquals(yield.frames()[1], expect2));
}

test(RealDashTest, ReadPartial) {
    MockYield yield(1);
    Frame expect(0x5800, 0, {0xf4, 0x08, 0x0e, 0xef, 0x39, 0x2c, 0x1b, 0x4c});
    byte buffer1[] = {
        0x66, 0x33, 0x22, 0x11,
        0x00, 0x58, 0x00, 0x00,
        0xf4, 0x08, 0x0e, 0xef,
    };
    byte buffer2[] = {
        0x39, 0x2c, 0x1b, 0x4c,
        0xf2, 0x30, 0x3f, 0x6e,
    };

    FakeReadStream stream;

    FakeRealDash realdash;
    realdash.begin(&stream);

    stream.set(buffer1, sizeof(buffer1)/sizeof(buffer1[0]));
    realdash.emit(yield.impl);
    assertEqual(yield.count(), 0);

    stream.set(buffer2, sizeof(buffer2)/sizeof(buffer2[0]));
    realdash.emit(yield.impl);
    assertEqual(yield.count(), 1);
    assertTrue(checkFrameEquals(yield.frames()[0], expect));
}

test(RealDashTest, ReadPreGarbage) {
    MockYield yield(1);
    Frame expect(0x5800, 0, {0xf4, 0x08, 0x0e, 0xef, 0x39, 0x2c, 0x1b, 0x4c});
    byte buffer[] = {
        0xf4, 0x08, 0x0e, 0xef,
        0x66, 0x33, 0x22, 0x11,
        0x00, 0x58, 0x00, 0x00,
        0xf4, 0x08, 0x0e, 0xef,
        0x39, 0x2c, 0x1b, 0x4c,
        0xf2, 0x30, 0x3f, 0x6e,
    };

    FakeReadStream stream;
    stream.set(buffer, sizeof(buffer)/sizeof(buffer[0]));

    FakeRealDash realdash;
    realdash.begin(&stream);

    realdash.emit(yield.impl);
    assertEqual(yield.count(), 1);
    assertTrue(checkFrameEquals(yield.frames()[0], expect));
}

test(RealDashTest, ReadPostGarbage) {
    MockYield yield(1);
    Frame expect(0x5800, 0, {0xf4, 0x08, 0x0e, 0xef, 0x39, 0x2c, 0x1b, 0x4c});
    byte buffer[] = {
        0x66, 0x33, 0x22, 0x11,
        0x00, 0x58, 0x00, 0x00,
        0xf4, 0x08, 0x0e, 0xef,
        0x39, 0x2c, 0x1b, 0x4c,
        0xf2, 0x30, 0x3f, 0x6e,
        0xf4, 0x08, 0x0e, 0xef,
    };

    FakeReadStream stream;
    stream.set(buffer, sizeof(buffer)/sizeof(buffer[0]));

    FakeRealDash realdash;
    realdash.begin(&stream);

    realdash.emit(yield.impl);
    assertEqual(yield.count(), 1);
    assertTrue(checkFrameEquals(yield.frames()[0], expect));

    realdash.emit(yield.impl);
    assertEqual(yield.count(), 1);
}

test(RealDashTest, Write) {
    Frame frame(0x5800, 8, {0xf4, 0x08, 0x0e, 0xef, 0x39, 0x2c, 0x1b, 0x4c});
    byte expect[] = {
        0x66, 0x33, 0x22, 0x11,
        0x00, 0x58, 0x00, 0x00,
        0xf4, 0x08, 0x0e, 0xef,
        0x39, 0x2c, 0x1b, 0x4c,
        0xf2, 0x30, 0x3f, 0x6e,
    };
    byte actual[20];
    size_t size = sizeof(actual)/sizeof(actual[0]);
    memset(actual, 0, size);

    FakeWriteStream stream;
    stream.set(actual, size);

    FakeRealDash realdash;
    realdash.begin(&stream);
    realdash.handle(frame);

    assertEqual(memcmp(actual, expect, size), 0);
}

#endif  // __R51_TESTS_TEST_REALDASH__
