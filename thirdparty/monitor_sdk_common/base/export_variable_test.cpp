//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 14:05
//  @file:      export_variable_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/export_variable.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

// Export global variable
int g_test_count;
EXPORT_VARIABLE(test_count, &g_test_count);

// Export global function
int CallCount()
{
    return g_test_count;
}
EXPORT_VARIABLE(call_count, CallCount);

TEST(ExportedVariables, GlobalTest)
{
    EXPECT_EQ("call_count = 0\ntest_count = 0\n", ExportedVariables::ToString());
    ++g_test_count;
    EXPECT_EQ("call_count = 1\ntest_count = 1\n", ExportedVariables::ToString());
}

class TestClass
{
public:
    TestClass() :
        m_field(1),
        m_const_method_reg("xxx", this, &TestClass::Xxx),
        m_non_const_method_reg("yyy", this, &TestClass::Yyy),
        m_reg_field("field", &m_field)
    {
    }
    std::string Xxx() const
    {
        return "xxx";
    }
    // non-const function
    std::string Yyy()
    {
        return "yyy";
    }
private:
    int m_field;
    VariableRegisterer m_const_method_reg;
    VariableRegisterer m_non_const_method_reg;
    VariableRegisterer m_reg_field;
};

TEST(ExportedVariables, ScopeTest)
{
    g_test_count = 0;
    EXPECT_EQ("call_count = 0\ntest_count = 0\n", ExportedVariables::ToString());
    {
        TestClass t;
        EXPECT_EQ("call_count = 0\nfield = 1\ntest_count = 0\nxxx = xxx\nyyy = yyy\n",
                  ExportedVariables::ToString());
    }
    EXPECT_EQ("call_count = 0\ntest_count = 0\n", ExportedVariables::ToString());
}

class TestClass2
{
public:
    TestClass2(VariableRegistry* registry = NULL) :
        m_field(1),
        m_variables(registry)
    {
        m_variables.Export("xxx", this, &TestClass2::Xxx);
        m_variables.Export("yyy", this, &TestClass2::Yyy);
        m_variables.Export("field", &m_field);
    }
    std::string Xxx() const
    {
        return "xxx";
    }
    // non-const function
    std::string Yyy()
    {
        return "yyy";
    }
private:
    int m_field;
    VariableExporter m_variables;
};

TEST(ExportedVariables, VariableExporter)
{
    g_test_count = 0;
    EXPECT_EQ("call_count = 0\ntest_count = 0\n", ExportedVariables::ToString());
    EXPECT_EQ("{\"call_count\":\"0\",\"test_count\":\"0\"}\n",
              ExportedVariables::ToStringAs("json"));
    {
        TestClass2 t;
        EXPECT_EQ("call_count = 0\nfield = 1\ntest_count = 0\nxxx = xxx\nyyy = yyy\n",
                  ExportedVariables::ToString());
        EXPECT_EQ("{\"call_count\":\"0\",\"field\":\"1\",\"test_count\":\"0\",\"xxx\":\"xxx\",\"yyy\":\"yyy\"}\n",
                  ExportedVariables::ToStringAs("json"));
    }
    EXPECT_EQ("call_count = 0\ntest_count = 0\n", ExportedVariables::ToString());
}

VariableRegistry g_test_registry;
EXPORT_VARIABLE_TO_REGISTRY(&g_test_registry, test_count_in_registry, &g_test_count);

TEST(ExportedVariables, GlobalVariableRegistry)
{
    EXPECT_EQ("test_count_in_registry = 0\n", g_test_registry.ToString());
}

TEST(ExportedVariables, VariableExporterWithRegistry)
{
    VariableRegistry registry;
    g_test_count = 0;
    EXPECT_EQ("", registry.ToString());
    {
        TestClass2 t(&registry);
        EXPECT_EQ("field = 1\nxxx = xxx\nyyy = yyy\n",
                  registry.ToString());
    }
    EXPECT_EQ("", registry.ToString());
}

TEST(ExportedVariables, SameNamedRegisterer)
{
    TestClass t1;
    TestClass t2;
}

TEST(ExportedVariables, SameNamedExporter)
{
    TestClass2 t1;
    TestClass2 t2;
}

TEST(ExportedVariables, Dump)
{
    g_test_count = 0;
    TestClass2 t;
    std::ostringstream os;
    std::vector<std::string> variables;
    // dump as text
    ExportedVariables::Dump(os, "text");
    EXPECT_EQ("call_count = 0\nfield = 1\ntest_count = 0\nxxx = xxx\nyyy = yyy\n",
              os.str());
    // dump as json
    os.str("");
    ExportedVariables::Dump(os, "json");
    EXPECT_EQ("{\"call_count\":\"0\",\"field\":\"1\",\"test_count\":\"0\",\"xxx\":\"xxx\",\"yyy\":\"yyy\"}\n",
              os.str());

    // dump specified variable
    variables.push_back("field");
    variables.push_back("xxx");
    os.str(""); // reset ostringstream
    ExportedVariables::Dump(os, "text", &variables);
    EXPECT_EQ("field = 1\nxxx = xxx\n",
              os.str());
    os.str("");
    ExportedVariables::Dump(os, "json", &variables);
    EXPECT_EQ("{\"field\":\"1\",\"xxx\":\"xxx\"}\n",
              os.str());

    // some dump variable is not exist
    variables.push_back("abc");
    os.str("");
    ExportedVariables::Dump(os, "text", &variables);
    EXPECT_EQ("field = 1\nxxx = xxx\nabc =  \n",
              os.str());
    os.str("");
    ExportedVariables::Dump(os, "json", &variables);
    // json output's key order is in dict order
    EXPECT_EQ("{\"abc\":\" \",\"field\":\"1\",\"xxx\":\"xxx\"}\n",
              os.str());
}

} // end of namespace common
