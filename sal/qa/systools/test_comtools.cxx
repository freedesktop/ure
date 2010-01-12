/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: test_comtools.cxx,v $
 * $Revision: 1.4 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sal.hxx"
// autogenerated file with codegen.pl

#include <testshl/simpleheader.hxx>
#include <systools/win32/comtools.hxx>

class COMObject : public IUnknown
{
public:
    COMObject() : ref_count_(0)
    {
    }

    ~COMObject()
    {
    }

    ULONG __stdcall AddRef()
    {
        ref_count_++;
        return ref_count_;
    }

    ULONG __stdcall Release()
    {
        ULONG cnt = --ref_count_;
        if (cnt == 0)
            delete this;
        return cnt;
    }

    HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppv)
    {
        if (riid == IID_IUnknown)
        {
            AddRef();
            *ppv = this;
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG GetRefCount() const
    {
        return ref_count_;
    }

private:
    ULONG ref_count_;
};

sal::systools::COMReference<IUnknown> comObjectSource()
{
    return sal::systools::COMReference<IUnknown>(new COMObject);
}

bool comObjectSink(sal::systools::COMReference<IUnknown> r, ULONG expectedRefCountOnReturn)
{
    r = sal::systools::COMReference<IUnknown>();
    COMObject* p = reinterpret_cast<COMObject*>(r.get());
    if (p)
        return (p->GetRefCount() == expectedRefCountOnReturn);
    else
        return (0 == expectedRefCountOnReturn);
}

void comObjectSource2(LPVOID* ppv)
{
    COMObject* p = new COMObject;
    p->AddRef();
    *ppv = p;
}

namespace test_comtools
{

    class test_COMReference : public CppUnit::TestFixture
    {

    public:
        /// test of COMReference<IUnknown> r;
        void default_ctor()
        {
            sal::systools::COMReference<IUnknown> r;
            CPPUNIT_ASSERT_MESSAGE("COMReference should be empty", r.get() == NULL);
        }

        void test_ctor_manual_AddRef()
        {
            COMObject* p = new COMObject;
            p->AddRef();
            sal::systools::COMReference<IUnknown> r(p, false);
            CPPUNIT_ASSERT_MESSAGE("Wrong reference count 1 is expected", reinterpret_cast<COMObject*>(r.get())->GetRefCount() == 1);
        }

        void test_copy_ctor()
        {
            sal::systools::COMReference<IUnknown> r(comObjectSource());
            CPPUNIT_ASSERT_MESSAGE("Wrong reference count 1 is expected", reinterpret_cast<COMObject*>(r.get())->GetRefCount() == 1);
        }

        void test_copy_assignment()
        {
            sal::systools::COMReference<IUnknown> r;
            CPPUNIT_ASSERT_MESSAGE("COMReference should be empty", r.get() == NULL);

            r = comObjectSource();
            CPPUNIT_ASSERT_MESSAGE("COMReference should be empty", r.get() != NULL);
            CPPUNIT_ASSERT_MESSAGE("Wrong reference count 1 is expected", reinterpret_cast<COMObject*>(r.get())->GetRefCount() == 1);
        }

        void test_ref_to_ref_assignment()
        {
            sal::systools::COMReference<IUnknown> r1 = comObjectSource();
            sal::systools::COMReference<IUnknown> r2 = r1;
            CPPUNIT_ASSERT_MESSAGE("Wrong reference count 2 is expected", reinterpret_cast<COMObject*>(r2.get())->GetRefCount() == 2);
        }

        void test_pointer_to_ref_assignment()
        {
            sal::systools::COMReference<IUnknown> r;
            r = new COMObject;
            CPPUNIT_ASSERT_MESSAGE("Wrong reference count 1 is expected", reinterpret_cast<COMObject*>(r.get())->GetRefCount() == 1);
        }

        void test_pointer_to_ref_assignment2()
        {
            sal::systools::COMReference<IUnknown> r = comObjectSource();
            r = new COMObject;
            CPPUNIT_ASSERT_MESSAGE("Wrong reference count 1 is expected", reinterpret_cast<COMObject*>(r.get())->GetRefCount() == 1);
        }

        void test_source_sink()
        {
            CPPUNIT_ASSERT_MESSAGE("Wrong reference count, 0 is expected", comObjectSink(comObjectSource(), 0));
        }

        void test_address_operator()
        {
            sal::systools::COMReference<IUnknown> r;
            comObjectSource2(reinterpret_cast<LPVOID*>(&r));
            CPPUNIT_ASSERT_MESSAGE("Wrong reference count, 1 is expected", reinterpret_cast<COMObject*>(r.get())->GetRefCount() == 1);
        }

        void test_address_operator2()
        {
            sal::systools::COMReference<IUnknown> r1 = comObjectSource();
            sal::systools::COMReference<IUnknown> r2 = r1;
            CPPUNIT_ASSERT_MESSAGE("Wrong reference count 2 is expected", reinterpret_cast<COMObject*>(r2.get())->GetRefCount() == 2);
            comObjectSource2(reinterpret_cast<LPVOID*>(&r1));
            CPPUNIT_ASSERT_MESSAGE("Wrong reference count 1 is expected", reinterpret_cast<COMObject*>(r1.get())->GetRefCount() == 1);
            CPPUNIT_ASSERT_MESSAGE("Wrong reference count 1 is expected", reinterpret_cast<COMObject*>(r2.get())->GetRefCount() == 1);
        }

        void test_clear()
        {
            sal::systools::COMReference<IUnknown> r = comObjectSource();
            CPPUNIT_ASSERT_MESSAGE("Wrong reference count 1 is expected", reinterpret_cast<COMObject*>(r.get())->GetRefCount() == 1);
            r.clear();
            CPPUNIT_ASSERT_MESSAGE("Expect reference to be empty", !r.is());
        }

        void test_query_interface()
        {
            try
            {
                sal::systools::COMReference<IUnknown> r1 = comObjectSource();
                sal::systools::COMReference<IUnknown> r2 = r1.QueryInterface<IUnknown>(IID_IUnknown);
                CPPUNIT_ASSERT_MESSAGE("Wrong reference count, 2 is expected", reinterpret_cast<COMObject*>(r2.get())->GetRefCount() == 2);
            }
            catch(sal::systools::ComError& ex)
            {
                CPPUNIT_ASSERT_MESSAGE("Exception should not have been thrown", false);
            }
        }

        void test_query_interface_throw()
        {
            try
            {
                sal::systools::COMReference<IUnknown> r1 = comObjectSource();
                sal::systools::COMReference<IPersistFile> r2 = r1.QueryInterface<IPersistFile>(IID_IPersistFile);
            }
            catch(sal::systools::ComError& ex)
            {
                return;
            }
            CPPUNIT_ASSERT_MESSAGE("Exception should have been thrown", false);
        }

        // Change the following lines only, if you add, remove or rename
        // member functions of the current class,
        // because these macros are need by auto register mechanism.

        CPPUNIT_TEST_SUITE(test_COMReference);
        CPPUNIT_TEST(default_ctor);
        CPPUNIT_TEST(test_ctor_manual_AddRef);
        CPPUNIT_TEST(test_copy_ctor);
        CPPUNIT_TEST(test_copy_assignment);
        CPPUNIT_TEST(test_ref_to_ref_assignment);
        CPPUNIT_TEST(test_pointer_to_ref_assignment);
        CPPUNIT_TEST(test_pointer_to_ref_assignment2);
        CPPUNIT_TEST(test_source_sink);
        CPPUNIT_TEST(test_address_operator);
        CPPUNIT_TEST(test_address_operator2);
        CPPUNIT_TEST(test_clear);
        CPPUNIT_TEST(test_query_interface);
        CPPUNIT_TEST(test_query_interface_throw);
        CPPUNIT_TEST_SUITE_END();
    };

// -----------------------------------------------------------------------------
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(test_comtools::test_COMReference, "test_comtools");

} // namespace rtl_OUString


// this macro creates an empty function, which will called by the RegisterAllFunctions()
// to let the user the possibility to also register some functions by hand.
NOADDITIONAL;

