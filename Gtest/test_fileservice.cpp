#include <gtest/gtest.h>
#include "FileService.h"
#include <QDir>

TEST(FileService, SafeName)
{
    EXPECT_TRUE(FileService::isSafeRelativeName("a.txt"));
    EXPECT_FALSE(FileService::isSafeRelativeName("../a.txt"));
    EXPECT_FALSE(FileService::isSafeRelativeName("/etc/passwd"));
}
