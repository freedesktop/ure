/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#include <testshl/simpleheader.hxx>

#include "cppuhelper/unourl.hxx"
#include "rtl/malformeduriexception.hxx"
#include "rtl/strbuf.hxx"
#include "rtl/string.h"
#include "rtl/textenc.h"
#include "rtl/ustring.hxx"
#include "sal/types.h"

namespace cppu_unourl
{
    class UrlTest : public CppUnit::TestFixture
    {
    public:
        void testDescriptorParsing()
        {
            struct Test
            {
                char const * pInput;
                bool bValid;
            };
            static Test const aTests[]
                = { { "", false },
                    { "abc", true },
                    { "Abc", true },
                    { "aBC", true },
                    { "ABC", true },
                    { "1abc", true },
                    { "123", true },
                    { "abc-1", false },
                    { "ab%63", false },
                    { "abc,", false },
                    { "abc,def=", true },
                    { "abc,Def=", true },
                    { "abc,DEF=", true },
                    { "abc,1def=", true },
                    { "abc,123=", true },
                    { "abc,def-1=", false },
                    { "abc,def", false },
                    { "abc,def=xxx,def=xxx", false },
                    { "abc,def=xxx,ghi=xxx", true },
                    { "abc,,def=xxx", false },
                    { "abc,def=xxx,,ghi=xxx", false },
                    { "abc,def=xxx,ghi=xxx,", false },
                    { "abc,def=%", true },
                    { "abc,def=%1", true },
                    { "abc,def=%00", true },
                    { "abc,def=%22", true },
                    { "abc,def=\"", true },
                    { "abc,def=%ed%a0%80", true } };
            for (int i = 0; i < sizeof aTests / sizeof (Test); ++i)
            {
                bool bValid = false;
                try
                {
                    cppu::UnoUrlDescriptor aDescriptor(rtl::OUString::createFromAscii(
                                                           aTests[i].pInput));
                    bValid = true;
                }
                catch (rtl::MalformedUriException &)
                {}

                if (aTests[i].bValid)
                {
                    CPPUNIT_ASSERT_MESSAGE("Valid uri parsed as invalid", bValid);
                }
                else
                {
                    CPPUNIT_ASSERT_MESSAGE("Invalid uri parsed as valid", !bValid);
                }
            }
        }

        void testDescriptorDescriptor()
        {
            struct Test
            {
                char const * pInput;
                char const * pDescriptor;
            };
            static Test const aTests[]
                = {{ "abc", "abc" },
                   { "Abc", "Abc" },
                   { "aBC", "aBC" },
                   { "ABC", "ABC" },
                   { "1abc", "1abc" },
                   { "123", "123" },
                   { "abc,def=", "abc,def=" },
                   { "abc,Def=", "abc,Def=" },
                   { "abc,DEF=", "abc,DEF=" },
                   { "abc,1def=", "abc,1def=" },
                   { "abc,123=", "abc,123=" },
                   { "abc,def=xxx,ghi=xxx", "abc,def=xxx,ghi=xxx" },
                   { "abc,def=%", "abc,def=%" },
                   { "abc,def=%1", "abc,def=%1" },
                   { "abc,def=%00", "abc,def=%00" },
                   { "abc,def=%22", "abc,def=%22" },
                   { "abc,def=\"", "abc,def=\"" },
                   { "abc,def=%ed%a0%80", "abc,def=%ed%a0%80" } };
            bool bResult = true;
            for (int i = 0; i < sizeof aTests / sizeof (Test); ++i)
            {
                bool bValid = false;
                rtl::OUString aDescriptor;
                try
                {
                    aDescriptor = cppu::UnoUrlDescriptor(rtl::OUString::createFromAscii(
                                                             aTests[i].pInput)).
                        getDescriptor();
                    bValid = true;
                }
                catch (rtl::MalformedUriException &)
                {}

                CPPUNIT_ASSERT_MESSAGE("Failed to parse URI", bValid);
                CPPUNIT_ASSERT_MESSAGE("Failed to parse URI correctly",
                                       aDescriptor.equalsAscii(
                                           aTests[i].pDescriptor));
            }
        }


        void testDescriptorName()
        {
            struct Test
            {
                char const * pInput;
                char const * pName;
            };
            static Test const aTests[]
                = { { "abc", "abc" },
                    { "Abc", "abc" },
                    { "aBC", "abc" },
                    { "ABC", "abc" },
                    { "1abc", "1abc" },
                    { "123", "123" },
                    { "abc,def=", "abc" },
                    { "abc,Def=", "abc" },
                    { "abc,DEF=", "abc" },
                    { "abc,1def=", "abc" },
                    { "abc,123=", "abc" },
                    { "abc,def=xxx,ghi=xxx", "abc" },
                    { "abc,def=%", "abc" },
                    { "abc,def=%1", "abc" },
                    { "abc,def=%00", "abc" },
                    { "abc,def=%22", "abc" },
                    { "abc,def=\"", "abc" },
                    { "abc,def=%ed%a0%80", "abc" } };
            for (int i = 0; i < sizeof aTests / sizeof (Test); ++i)
            {
                bool bValid = false;
                rtl::OUString aName;
                try
                {
                    aName = cppu::UnoUrlDescriptor(rtl::OUString::createFromAscii(
                                                       aTests[i].pInput)).getName();
                    bValid = true;
                }
                catch (rtl::MalformedUriException &)
                {}

                CPPUNIT_ASSERT_MESSAGE("Failed to parse URI", bValid);
                CPPUNIT_ASSERT_MESSAGE("Failed to parse URI correctly",
                                       aName.equalsAscii(aTests[i].pName));
            }
        }

        void testDescriptorKey(void)
        {
            struct Test
            {
                char const * pInput;
                char const * pKey;
                bool bPresent;
            };
            static Test const aTests[]
                = { { "abc", "abc", false },
                    { "abc", "def", false },
                    { "1abc", "def", false },
                    { "123", "def", false },
                    { "abc,def=", "abc", false },
                    { "abc,def=", "def", true },
                    { "abc,def=", "defg", false },
                    { "abc,def=", "de", false },
                    { "abc,def=", "ghi", false },
                    { "abc,Def=", "def", true },
                    { "abc,Def=", "Def", true },
                    { "abc,Def=", "dEF", true },
                    { "abc,Def=", "DEF", true },
                    { "abc,def=xxx,ghi=xxx", "abc", false },
                    { "abc,def=xxx,ghi=xxx", "def", true },
                    { "abc,def=xxx,ghi=xxx", "ghi", true },
                    { "abc,def=xxx,ghi=xxx", "jkl", false } };
            for (int i = 0; i < sizeof aTests / sizeof (Test); ++i)
            {
                bool bValid = false;
                bool bPresent = false;
                try
                {
                    bPresent = cppu::UnoUrlDescriptor(rtl::OUString::createFromAscii(
                                                          aTests[i].pInput)).
                        hasParameter(rtl::OUString::createFromAscii(aTests[i].pKey));
                    bValid = true;
                }
                catch (rtl::MalformedUriException &)
                {}

                CPPUNIT_ASSERT_MESSAGE("Failed to parse URI", bValid);
                CPPUNIT_ASSERT_MESSAGE("Failed to detect parameter correctly",
                                       bPresent == aTests[i].bPresent);
            }
        }

        void testDescriptorValue()
        {
            struct Test
            {
                char const * pInput;
                char const * pKey;
                char const * pValue;
            };
            static Test const aTests[]
                = { { "abc", "abc", "" },
                    { "abc", "def", "" },
                    { "1abc", "def", "" },
                    { "123", "def", "" },
                    { "abc,def=", "abc", "" },
                    { "abc,def=", "def", "" },
                    { "abc,def=", "defg", "" },
                    { "abc,def=", "de", "" },
                    { "abc,def=", "ghi", "" },
                    { "abc,Def=", "def", "" },
                    { "abc,Def=", "Def", "" },
                    { "abc,Def=", "dEF", "" },
                    { "abc,Def=", "DEF", "" },
                    { "abc,def=xxx,ghi=xxx", "abc", "" },
                    { "abc,def=xxx,ghi=xxx", "def", "xxx" },
                    { "abc,def=xxx,ghi=xxx", "ghi", "xxx" },
                    { "abc,def=xxx,ghi=xxx", "jkl", "" },
                    { "abc,def=%", "def", "%" },
                    { "abc,def=%1", "def", "%1" },
                    { "abc,def=%22", "def", "\"" },
                    { "abc,def=\"", "def", "\"" },
                    { "abc,def=abc", "def", "abc" },
                    { "abc,def=Abc", "def", "Abc" },
                    { "abc,def=aBC", "def", "aBC" },
                    { "abc,def=ABC", "def", "ABC" },
                    { "abc,def=%,ghi=", "def", "%" },
                    { "abc,def=%1,ghi=", "def", "%1" },
                    { "abc,def=%22,ghi=", "def", "\"" },
                    { "abc,def=\",ghi=", "def", "\"" },
                    { "abc,def=abc,ghi=", "def", "abc" },
                    { "abc,def=Abc,ghi=", "def", "Abc" },
                    { "abc,def=aBC,ghi=", "def", "aBC" },
                    { "abc,def=ABC,ghi=", "def", "ABC" },
                    { "abc,abc=,def=%", "def", "%" },
                    { "abc,abc=,def=%1", "def", "%1" },
                    { "abc,abc=,def=%22", "def", "\"" },
                    { "abc,abc=,def=\"", "def", "\"" },
                    { "abc,abc=,def=abc", "def", "abc" },
                    { "abc,abc=,def=Abc", "def", "Abc" },
                    { "abc,abc=,def=aBC", "def", "aBC" },
                    { "abc,abc=,def=ABC", "def", "ABC" } };
            for (int i = 0; i < sizeof aTests / sizeof (Test); ++i)
            {
                bool bValid = false;
                rtl::OUString aValue;
                try
                {
                    aValue = cppu::UnoUrlDescriptor(rtl::OUString::createFromAscii(
                                                        aTests[i].pInput)).
                        getParameter(rtl::OUString::createFromAscii(aTests[i].pKey));
                    bValid = true;
                }
                catch (rtl::MalformedUriException &)
                {}
                CPPUNIT_ASSERT_MESSAGE("Failed to parse URI", bValid);
                CPPUNIT_ASSERT_MESSAGE("Failed to get param correctly",
                                       aValue.equalsAscii(aTests[i].pValue));
            }
        }

        void testUrlParsing()
        {
            struct Test
            {
                char const * pInput;
                bool bValid;
            };
            static Test const aTests[]
                = { { "", false },
                    { "abc", false },
                    { "uno", false },
                    { "uno:", false },
                    { "uno:abc;def;ghi", true },
                    { "Uno:abc;def;ghi", true },
                    { "uNO:abc;def;ghi", true },
                    { "UNO:abc;def;ghi", true },
                    { "uno:abc,def=xxx,ghi=xxx;def,ghi=xxx,jkl=xxx;ghi", true },
                    { "uno:abc,def=xxx,ghi=xxx;def,ghi=xxx,jkl=xxx,;ghi", false },
                    { "uno:abc;def;", false },
                    { "uno:abc;def;a", true },
                    { "uno:abc;def;A", true },
                    { "uno:abc;def;1", true },
                    { "uno:abc;def;$&+,/:=?@", true },
                    { "uno:abc;def;%24&+,/:=?@", false } };
            for (int i = 0; i < sizeof aTests / sizeof (Test); ++i)
            {
                bool bValid = false;
                try
                {
                    cppu::UnoUrl aUrl(rtl::OUString::createFromAscii(aTests[i].pInput));
                    bValid = true;
                }
                catch (rtl::MalformedUriException &)
                {}

                if (aTests[i].bValid)
                {
                    CPPUNIT_ASSERT_MESSAGE("Valid uri parsed as invalid", bValid);
                }
                else
                {
                    CPPUNIT_ASSERT_MESSAGE("Invalid uri parsed as valid", !bValid);
                }

            }
        }

        void testUrlConnection()
        {
            struct Test
            {
                char const * pInput;
                char const * pConnection;
            };
            static Test const aTests[]
                = { { "uno:abc;def;ghi", "abc" },
                    { "uno:Abc;def;ghi", "Abc" },
                    { "uno:aBC;def;ghi", "aBC" },
                    { "uno:ABC;def;ghi", "ABC" },
                    { "uno:abc,def=xxx,ghi=xxx;def,ghi=xxx,jkl=xxx;ghi",
                      "abc,def=xxx,ghi=xxx" } };
            for (int i = 0; i < sizeof aTests / sizeof (Test); ++i)
            {
                bool bValid = false;
                rtl::OUString aConnection;
                try
                {
                    aConnection = cppu::UnoUrl(rtl::OUString::createFromAscii(
                                                   aTests[i].pInput)).
                        getConnection().getDescriptor();
                    bValid = true;
                }
                catch (rtl::MalformedUriException &)
                {}
                CPPUNIT_ASSERT_MESSAGE("Failed to parse URI", bValid);
                CPPUNIT_ASSERT_MESSAGE("Failed to get param correctly",
                                       aConnection.equalsAscii(
                                           aTests[i].pConnection));
            }
        }

        void testUrlProtocol()
        {
            struct Test
            {
                char const * pInput;
                char const * pProtocol;
            };
            static Test const aTests[]
                = { { "uno:abc;def;ghi", "def" },
                    { "uno:abc;Def;ghi", "Def" },
                    { "uno:abc;dEF;ghi", "dEF" },
                    { "uno:abc;DEF;ghi", "DEF" },
                    { "uno:abc,def=xxx,ghi=xxx;def,ghi=xxx,jkl=xxx;ghi",
                      "def,ghi=xxx,jkl=xxx" } };
            for (int i = 0; i < sizeof aTests / sizeof (Test); ++i)
            {
                bool bValid = false;
                rtl::OUString aProtocol;
                try
                {
                    aProtocol = cppu::UnoUrl(rtl::OUString::createFromAscii(
                                                 aTests[i].pInput)).
                        getProtocol().getDescriptor();
                    bValid = true;
                }
                catch (rtl::MalformedUriException &)
                {}
                CPPUNIT_ASSERT_MESSAGE("Failed to parse URI", bValid);
                CPPUNIT_ASSERT_MESSAGE("Failed to get protocol correctly",
                                       aProtocol.equalsAscii(
                                           aTests[i].pProtocol));
            }
        }

        void testUrlObjectName()
        {
            struct Test
            {
                char const * pInput;
                char const * pObjectName;
            };
            static Test const aTests[]
                = { { "uno:abc;def;ghi", "ghi" },
                    { "uno:abc;def;Ghi", "Ghi" },
                    { "uno:abc;def;gHI", "gHI" },
                    { "uno:abc;def;GHI", "GHI" },
                    { "uno:abc,def=xxx,ghi=xxx;def,ghi=xxx,jkl=xxx;ghi", "ghi" },
                    { "uno:abc;def;a", "a" },
                    { "uno:abc;def;A", "A" },
                    { "uno:abc;def;1", "1" },
                    { "uno:abc;def;$&+,/:=?@", "$&+,/:=?@" } };
            for (int i = 0; i < sizeof aTests / sizeof (Test); ++i)
            {
                bool bValid = false;
                rtl::OUString aObjectName;
                try
                {
                    aObjectName = cppu::UnoUrl(rtl::OUString::createFromAscii(
                                                   aTests[i].pInput)).getObjectName();
                    bValid = true;
                }
                catch (rtl::MalformedUriException &)
                {}
                CPPUNIT_ASSERT_MESSAGE("Failed to parse URI", bValid);
                CPPUNIT_ASSERT_MESSAGE("Failed to get protocol correctly",
                                       aObjectName.equalsAscii(
                                           aTests[i].pObjectName));
            }
        }

        // Automatic registration code
        CPPUNIT_TEST_SUITE(UrlTest);
        CPPUNIT_TEST(testDescriptorParsing);
        CPPUNIT_TEST(testDescriptorDescriptor);
        CPPUNIT_TEST(testDescriptorName);
        CPPUNIT_TEST(testDescriptorKey);
        CPPUNIT_TEST(testDescriptorValue);
        CPPUNIT_TEST(testUrlParsing);
        CPPUNIT_TEST(testUrlConnection);
        CPPUNIT_TEST(testUrlProtocol);
        CPPUNIT_TEST(testUrlObjectName);
        CPPUNIT_TEST_SUITE_END();
    };
} // namespace cppu_ifcontainer

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(cppu_unourl::UrlTest,
                                      "cppu_unourl");

NOADDITIONAL;

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
