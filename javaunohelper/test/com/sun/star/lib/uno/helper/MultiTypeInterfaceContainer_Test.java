/*************************************************************************
 *
 *  $RCSfile: MultiTypeInterfaceContainer_Test.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jl $ $Date: 2002-04-11 13:43:14 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

package com.sun.star.lib.uno.helper;
import com.sun.star.lib.uno.environments.java.java_environment;
import java.util.List;
import com.sun.star.uno.XWeak;
import com.sun.star.lang.XTypeProvider;
import java.util.ArrayList;
import com.sun.star.uno.Type;
import com.sun.star.lib.uno.typedesc.TypeDescription;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.lib.uno.environments.java.Proxy;
import com.sun.star.uno.XInterface;
import com.sun.star.lang.XSingleComponentFactory;

public class MultiTypeInterfaceContainer_Test
{
    java_environment env= new java_environment(null);
    /** Creates a new instance of InterfaceContainerTest */
    AWeakBase obj1,obj2,obj3,obj4;
    Object proxyObj1Weak1;
    Object proxyObj3Weak1;
    Object proxyObj3Weak2;
    Object proxyObj3TypeProv;
    Object proxyObj2TypeProv;
    //contains original objects
    List list1;
    //contains original objects + proxies
    List list2;
    //contains original object + proxies + null value
    List list3;

    /** Creates a new instance of MultiTypeInterfaceContainer_Test */
    public MultiTypeInterfaceContainer_Test()
    {
        obj1= new AWeakBase();
        obj2= new AWeakBase();
        obj3= new AWeakBase();
        obj4= new AWeakBase();
        proxyObj1Weak1= ProxyProvider.getHolderProxy(obj1, XWeak.class);
        proxyObj3Weak1= ProxyProvider.getHolderProxy(obj3, XWeak.class);
        proxyObj3Weak2= ProxyProvider.getHolderProxy(obj3, XWeak.class);
        proxyObj2TypeProv= ProxyProvider.getHolderProxy(obj2, XTypeProvider.class);
        proxyObj3TypeProv= ProxyProvider.getHolderProxy(obj3, XTypeProvider.class);
        
        list1= new ArrayList();
        list1.add(obj1);
        list1.add(obj2);
        list1.add(obj3);
        list2= new ArrayList();
        list2.add(obj1);
        list2.add(proxyObj2TypeProv);
        list2.add(proxyObj3TypeProv);
        list3= new ArrayList();
        list3.add(obj1);
        list3.add(null);
        list3.add(proxyObj2TypeProv);
        list3.add(proxyObj3Weak1);
    }
    /** returns Holder proxy objects for the specified interface. If the method is called
     *  several times with the same arguments then each time a new HolderProxy is returned.
     *  Then all HolderProxy s refer to the same Proxy object.
     *  The proxy can be queried for XEventListener. On the returned proxy disposing can be called
     *
     */
    public Object getHolderProxy(Object obj, Class iface)
    {
        Object retVal= null;
        if (obj == null || iface == null || iface.isInstance(obj) == false )
            return retVal;
        
        Type type= new Type(TypeDescription.getTypeDescription(iface));
        Type evtType= new Type(TypeDescription.getTypeDescription(com.sun.star.lang.XEventListener.class));
        // find the object identifier
        String sOid= UnoRuntime.generateOid(obj);
        retVal= env.getRegisteredInterface(sOid, type);
        // if retVal == null then probably not registered
        if (retVal == null)
        {
            // create the XEventListener proxy
            Requester eventRequester = new Requester(false, false, null);
            Object aProxyEvt = Proxy.create(eventRequester, sOid, evtType, false, false);
            String[] arOid= new String[]{sOid};
            retVal= env.registerInterface(aProxyEvt, arOid, evtType);
            
            Requester requester = new Requester(false, false, aProxyEvt);
            Object aProxy = Proxy.create(requester, sOid, type, false, false);
            arOid= new String[] {sOid};
            retVal= env.registerInterface(aProxy, arOid, type);
        }
        return retVal;
    }

    public boolean addInterface()
    {
        System.out.println("Testing MultiTypeInterfaceContainer.addInterface");
        MultiTypeInterfaceContainer cont= new MultiTypeInterfaceContainer();
        boolean r[]= new boolean[200];
        int i= 0;
        
        int ci= 0;
        ci= cont.addInterface(new Type(XInterface.class), obj1);
        ci= cont.addInterface(new Type(XInterface.class), obj2);
        ci= cont.addInterface(new Type(XInterface.class), obj3);
        r[i++]= ci == 3;
        ci= cont.addInterface(new Type(XWeak.class), obj1);
        ci= cont.addInterface(new Type(XWeak.class), obj2);
        r[i++]= ci ==2;
        ci= cont.addInterface(null,obj1);
        r[i++]= ci == 1;
        ci= cont.addInterface(new Type(XTypeProvider.class), null);
        r[i++]= ci == 0;
        
        cont= new MultiTypeInterfaceContainer();
        AWeakBase[] arObj= new AWeakBase[100];
        for (int c= 0; c < 100; c++)
        {
            arObj[c]= new AWeakBase();
            ci= cont.addInterface(new Type(XInterface.class), arObj[c]);
        }
        Type[] arT= cont.getContainedTypes();
        for (int c=0; c < 100; c++)
        {
            ci= cont.removeInterface(new Type(XInterface.class), arObj[c]);
            r[i++]= ci == 100 -c -1;
            
        }
        boolean bOk= true;
        for (int c= 0; c < i; c++)
            bOk= bOk && r[c];
        if (bOk == false)
            System.out.println("Failed");
        else
            System.out.println("Ok");
        return bOk;
    }
    
    public boolean getContainedTypes()
    {
        System.out.println("Testing MultiTypeInterfaceContainer.getContainedTypes");
        MultiTypeInterfaceContainer cont= new MultiTypeInterfaceContainer();
        boolean r[]= new boolean[50];
        int i= 0;
        
        cont.addInterface(new Type(XInterface.class), obj1);
        cont.addInterface(new Type(XWeak.class), obj1);
        cont.addInterface(null, obj1);
        cont.addInterface(new Type(XTypeProvider.class), null);
        Object aObj= new Object();
        cont.addInterface(aObj, obj1);
        cont.addInterface(XSingleComponentFactory.class, obj1);
        Type[] types= cont.getContainedTypes();
        // 3 types and no XTypeProvider
        r[i++]= types.length == 5;
        for (int c= 0; c < types.length; c++)
        {
            if (types[c] == null)
                r[i++]= true;
            else if(types[c].equals( new Type(XTypeProvider.class)))
                r[i++]= false;
            else if(types[c].equals(new Type(XInterface.class)))
                r[i++]= true;
            else if (types[c].equals(new Type(XWeak.class)))
                r[i++]= true;
            else if (types[c].equals(new Type()))
                r[i++]= true;
            else if (types[c].equals(new Type( aObj.getClass())))
                r[i++]= true;
            else if (types[c].equals(new Type(XSingleComponentFactory.class)))
                r[i++]= true;
            else
                r[i++]= false;
        }
        boolean bOk= true;
        for (int c= 0; c < i; c++)
            bOk= bOk && r[c];
        if (bOk == false)
            System.out.println("Failed");
        else
            System.out.println("Ok");
        return bOk;
    }

    public boolean getContainer()
    {
        System.out.println("Testing MultiTypeInterfaceContainer.getContainedTypes");
        MultiTypeInterfaceContainer cont= new MultiTypeInterfaceContainer();
        boolean r[]= new boolean[50];
        int i= 0;

        int ci= 0;
        ci= cont.addInterface(new Type(XInterface.class), obj1);
        ci= cont.addInterface(new Type(XInterface.class), obj2);
        ci= cont.addInterface(new Type(XInterface.class), obj3);
        ci= cont.addInterface(new Type(XWeak.class), obj1);
        ci= cont.addInterface(new Type(XWeak.class), obj2);
        ci= cont.addInterface(null, obj1);
        ci= cont.addInterface(new Type(XTypeProvider.class), null);

        InterfaceContainer icont= null;
        icont= cont.getContainer( new Type(XTypeProvider.class));
        r[i++]= icont.size() == 0;
        icont= cont.getContainer(new Type(XWeak.class));
        r[i++]= icont.size() == 2;
        icont= cont.getContainer(null);
        r[i++]= icont.size() == 1;
        
        boolean bOk= true;
        for (int c= 0; c < i; c++)
            bOk= bOk && r[c];
        if (bOk == false)
            System.out.println("Failed");
        else
            System.out.println("Ok");
        return bOk;
    }

    public boolean removeInterface()
    {
        System.out.println("Testing MultiTypeInterfaceContainer.removeInterface");
        MultiTypeInterfaceContainer cont= new MultiTypeInterfaceContainer();
        boolean r[]= new boolean[50];
        int i= 0;

        int count= 0;
        count= cont.removeInterface( new Type(XTypeProvider.class), obj1);
        r[i++]= count == 0;
        count= cont.removeInterface( new Type(XTypeProvider.class), null);
        r[i++]= count == 0;
        count= cont.removeInterface(null, obj2);
        r[i++]= count == 0;
        
        cont.addInterface(new Type(XInterface.class), obj1);
        cont.addInterface(null, obj1);
        count= cont.removeInterface(null, obj2);
        // count must still be 1
        r[i++]= count == 1;
        count= cont.removeInterface(null, obj1);
        r[i++]= count == 0;
        count= cont.removeInterface(new Type(XInterface.class), obj1);
        r[i++]= count == 0;
        
          boolean bOk= true;
        for (int c= 0; c < i; c++)
            bOk= bOk && r[c];
        if (bOk == false)
            System.out.println("Failed");
        else
            System.out.println("Ok");
        return bOk;
    }

    public boolean clear()
    {
        System.out.println("Testing MultiTypeInterfaceContainer.clear");
        MultiTypeInterfaceContainer cont= new MultiTypeInterfaceContainer();
        boolean r[]= new boolean[50];
        int i= 0;

        int count= 0;
        cont.clear();
        Type[] types= cont.getContainedTypes();
        r[i++]= types.length == 0;
        int ci;
        ci= cont.addInterface(new Type(XInterface.class), obj1);
        ci= cont.addInterface(new Type(XInterface.class), obj2);
        ci= cont.addInterface(new Type(XInterface.class), obj3);
        ci= cont.addInterface(new Type(XWeak.class), obj1);
        ci= cont.addInterface(new Type(XWeak.class), obj2);
        ci= cont.addInterface(null, obj1);
        ci= cont.addInterface(new Type(XTypeProvider.class), null);
        types= cont.getContainedTypes();
        r[i++]= types.length == 3;
        cont.clear();
        types= cont.getContainedTypes();
        r[i++]= types.length == 0;
            
        boolean bOk= true;
        for (int c= 0; c < i; c++)
            bOk= bOk && r[c];
        if (bOk == false)
            System.out.println("Failed");
        else
            System.out.println("Ok");
        return bOk;
    }

    public boolean disposeAndClear()
    {
        System.out.println("Testing MultiTypeInterfaceContainer.disposeAndClear");
        MultiTypeInterfaceContainer cont= new MultiTypeInterfaceContainer();
        boolean r[]= new boolean[50];
        int i= 0;
        obj1.nDisposingCalled= 0;
        obj2.nDisposingCalled= 0;
        obj3.nDisposingCalled= 0;
        cont.addInterface(new Type(XInterface.class), null);
        cont.addInterface(new Type(XInterface.class), obj1);
        cont.addInterface(new Type(XInterface.class), obj2);
        cont.addInterface(new Type(XInterface.class), obj3);
        cont.addInterface(new Type(XWeak.class),obj1);
        cont.addInterface(new Type(XWeak.class), obj2);
        cont.addInterface(new Type(XTypeProvider.class), obj1);
        cont.disposeAndClear(new com.sun.star.lang.EventObject("blabla"));
        
        r[i++]= obj1.nDisposingCalled == 3;
        r[i++]= obj2.nDisposingCalled == 2;
        r[i++]= obj3.nDisposingCalled == 1;
        Type[] types= cont.getContainedTypes();
        r[i++]= types.length == 0;
        
        boolean bOk= true;
        for (int c= 0; c < i; c++)
            bOk= bOk && r[c];
        if (bOk == false)
            System.out.println("Failed");
        else
            System.out.println("Ok");
        return bOk;
    }
    
    
    public static void main(String[] args)
    {
        MultiTypeInterfaceContainer_Test test= new MultiTypeInterfaceContainer_Test();
        boolean r[]= new boolean[50];
        int i= 0;
        r[i++]= test.addInterface();
        r[i++]= test.getContainedTypes();
        r[i++]= test.getContainer();
        r[i++]= test.removeInterface();
        r[i++]= test.clear();
        r[i++]= test.disposeAndClear();
        boolean bOk= true;
        for (int c= 0; c < i; c++)
            bOk= bOk && r[c];
        if ( ! bOk )
            System.out.println("Test finished.\nErrors occured!!!");
        else
            System.out.println("Test finished. \nNo errors.");
   
    }    
}



