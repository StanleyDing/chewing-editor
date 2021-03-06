/*
 * chewing-editor: Chewing userphrase editor
 * Copyright (C) 2014 Chewing Development Team

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "gtest/gtest.h"

#include <QDebug>
#include <QDir>

#include "ChewingImporter.h"

class ChewingImporterTest : public ::testing::Test {
protected:
    ChewingImporterTest() = default;
    virtual ~ChewingImporterTest() = default;
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(ChewingImporterTest, ReadNoUserphrase)
{
    auto list = QDir{TESTDATA "/import/broken"}.entryList();

    foreach(auto file, list) {
        auto path = QString("%1/%2").arg(TESTDATA "/import/broken").arg(file);
        ChewingImporter importer{path};
        auto result = importer.load();
        EXPECT_FALSE(result.first);
        ASSERT_EQ(0, result.second.size());
    }
}

TEST_F(ChewingImporterTest, ReadOneUserphrase)
{
    ChewingImporter importer{QString(TESTDATA "/import/chewing_one_valid_phrase.json")};

    auto result = importer.load();

    EXPECT_TRUE(result.first);
    ASSERT_EQ(1, result.second.size());
    EXPECT_EQ(0, QString::compare(
        QString("\xE6\xB8\xAC\xE8\xA9\xA6" /* 測試 */),
        result.second[0].phrase_));
    EXPECT_EQ(0, QString::compare(
        QString("\xE3\x84\x98\xE3\x84\x9C\xCB\x8B \xE3\x84\x95\xCB\x8B" /* ㄘㄜˋ ㄕˋ */),
        result.second[0].bopomofo_));
}

TEST_F(ChewingImporterTest, PathError)
{
    ChewingImporter importer{TESTDATA "/NoSuchPath/chewing.json"};

    auto result = importer.load();

    EXPECT_FALSE(result.first);
    ASSERT_EQ(0, result.second.size());
}
