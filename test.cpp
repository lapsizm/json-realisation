#include "json.h"
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
TEST(JSON, jTest){
    /*std::ifstream fin("../test_file3.txt");
    Document doc = Load(fin);
    Node node = doc.GetRoot();
    EXPECT_EQ(node.IsArray(), true);
    EXPECT_EQ(node.GiveArray().size(), 2);
    EXPECT_EQ(node.GiveArray()[0].GiveDict().size(), 4);
    auto i = node.GiveArray()[0].GiveDict().find("age");
    EXPECT_EQ(i->first, "age");
    EXPECT_EQ(i->second.AsInt(), 29);

    auto i_ = node.GiveArray()[0].GiveDict().find("powers");
    EXPECT_EQ(i_->first, "powers");
    EXPECT_EQ(i_->second.IsArray(), true);
    EXPECT_EQ(i_->second.GiveArray()[2].AsString(), "Radiation blast");

    auto temp = node.GiveArray()[1].GiveDict().find("question");
    EXPECT_EQ(temp->first, "question");
    EXPECT_EQ(temp->second.IsBool(), true);
    EXPECT_EQ(temp->second.AsBool(), true);
    fin.close();*/
}

