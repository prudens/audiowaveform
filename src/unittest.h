#pragma once
#include <vector>
#include <iostream>
#include "consolecolor.h"
#include <string>
using namespace std;

class RunUnitTest
{
public:
    RunUnitTest() :m_bPass( false ) {}
    virtual ~RunUnitTest() {}
    virtual void Run() = 0;
    virtual void SetUp() = 0;
    virtual void TearDown() = 0;
    virtual void Pass( bool bPass ) { m_bPass = bPass; }
    virtual bool isPass()const { return m_bPass; }
private:
    bool m_bPass;
};

class UnitTestManager
{
public:
    static UnitTestManager& GetInstance()
    {
        static UnitTestManager s_instance;
        return s_instance;
    }
    RunUnitTest* RegisterUnitCase( RunUnitTest* t )
    {
        _unit_cases.push_back( t );
        return t;
    }
    void RunAllTest()
    {
        std::cout << yellow << "gtest_lite version:0.0.1" << std::endl;
        std::cout << blue << "[  CASE  ]There are " << _unit_cases.size() << " case to test" << std::endl;
        int nPass = 0, nNotPass = 0;
        for ( auto& uc : _unit_cases )
        {
            uc->SetUp();
            uc->Run();
            uc->TearDown();
            if ( uc->isPass() )
            {
                nPass++;
            }
            else
            {
                nNotPass++;
            }
        }

        std::cout << blue << "[  END   ]Have Test " << nPass << " Pass and " << nNotPass << " Failed  " << std::endl;
        std::cout << reset;
    }
private:

    std::vector<RunUnitTest*>_unit_cases;
};


class NullType{};

#define CASENAME(class_type) class_type##case_name##_UnitTest

#define CASE_TEST(class_type,case_name,...)       \
class CASENAME(class_type) :public RunUnitTest           \
 {           \
class_type *t; \
const std::string m_class_type = #class_type;\
const std::string m_class_name = #case_name;\
public:\
CASENAME(class_type)(){}\
~CASENAME(class_type)() { if ( t )delete t; }\
void SetUp(){t = new class_type (__VA_ARGS__);cout<<green <<"[  RUN   ]"<<m_class_type<<"."<<m_class_name<<endl;}\
void TearDown() {\
if ( t )delete t; t = nullptr;\
if(isPass()){ cout<<green << "[  OK    ]"<<m_class_type<<"."<<m_class_name<<endl; }\
else        { cout << red << "[  FAIL  ]" << m_class_type <<"."<< m_class_name << endl; }}\
void Run();\
};\
CASENAME(class_type) *class_type##case_name = static_cast<CASENAME(class_type)*>( UnitTestManager::GetInstance().RegisterUnitCase( new CASENAME(class_type) ) );\
void CASENAME(class_type)::Run()

#define RUN_ALL_TEST() UnitTestManager::GetInstance().RunAllTest();

#define EXPECT_EQ(expect,actual) \
Pass(expect != actual);\
if(!isPass())\
{\
    std::cout << red <<"[        ]"<<__FILE__<<"("<<__LINE__<< "): :" << #actual<<std::endl; \
    std::cout << red << "          Expect:" << expect << std::endl; \
    std::cout << red << "          Actual:" << actual << std::endl; \
}

#define EXPECT_MF_EQ(expect,actual) \
Pass(expect != t->actual);\
if(!isPass())\
{\
    std::cout << red <<"[        ]"<<__FILE__<<"("<<__LINE__<< "): :" << #actual<<std::endl; \
    std::cout << red << "          Expect:" << expect << std::endl; \
    std::cout << red << "          Actual:" << t->actual << std::endl; \
}



//这里继续补充其他断言格式
