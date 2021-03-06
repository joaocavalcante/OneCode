/******************************** 模块头 ***********************************\
模块名:  CppCLINETAssemblyWrapper.cpp
项目:    CppCLINETAssemblyWrapper
版权：   (c) Microsoft Corporation.

文件中的代码声明了C++的类CSSimpleObjectWrapper封装在.NET类库CSClassLibrary
中定义的类CSSimpleObject。本地的C++应用程序可以引用这个封装类并链接DLL去间
接调用.NET类。

  CppCallNETAssemblyWrapper (本地C++应用程序)
          -->
      CppCLINETAssemblyWrapper (C++/CLI 封装)
              -->
          CSClassLibrary (.NET 程序集)

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes
#include "CppCLINETAssemblyWrapper.h"

#include <msclr/marshal.h>
using namespace msclr::interop;

#include <strsafe.h>

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace CSClassLibrary;
#pragma endregion


CSSimpleObjectWrapper::CSSimpleObjectWrapper(void)
{
    // 实例化C#的类CSSimpleObject.
    CSSimpleObject ^ obj = gcnew CSSimpleObject();

    // 绑定CSSimpleObject的.NET对象, 并且在m_impl中
    // 记录绑定对象的地址
    m_impl = GCHandle::ToIntPtr(GCHandle::Alloc(obj)).ToPointer(); 
}

CSSimpleObjectWrapper::~CSSimpleObjectWrapper(void)
{
    // 获取与GCHandle相关联的绑定对象的地址 
    // 并且释放GCHandle。在这点上， CSSimpleObject对象对GC是适当的。
    GCHandle h = GCHandle::FromIntPtr(IntPtr(m_impl));
    h.Free();
}

float CSSimpleObjectWrapper::get_FloatProperty(void)
{
    // 从内存地址中获得绑定对象CSSimpleObject
    GCHandle h = GCHandle::FromIntPtr(IntPtr(m_impl));
    CSSimpleObject ^ obj = safe_cast<CSSimpleObject^>(h.Target);

    // 返回CSSimpleObject 对象的属性.
    return obj->FloatProperty;
}

void CSSimpleObjectWrapper::set_FloatProperty(float fVal)
{
    // 从内存地址中获得绑定对象CSSimpleObject
    GCHandle h = GCHandle::FromIntPtr(IntPtr(m_impl));
    CSSimpleObject ^ obj = safe_cast<CSSimpleObject^>(h.Target);

    // 重新使用绑定对象CSSimpleObject中的相同属性的调用
    obj->FloatProperty = fVal;
}

HRESULT CSSimpleObjectWrapper::ToString(PWSTR pszBuffer, DWORD dwSize)
{
    // 从内存地址中获得绑定对象CSSimpleObject
    GCHandle h = GCHandle::FromIntPtr(IntPtr(m_impl));
    CSSimpleObject ^ obj = safe_cast<CSSimpleObject^>(h.Target);

    String ^ str;
    HRESULT hr;
    try
    {
        // 重新使用绑定对象CSSimpleObject中的相同属性的调用
        str = obj->ToString();
    }
    catch (Exception ^ e)
    {
        hr = Marshal::GetHRForException(e);
    }

    if (SUCCEEDED(hr))
    {
        // 从System::String到PCWSTR的转换.
        marshal_context ^ context = gcnew marshal_context();
        PCWSTR pszStr = context->marshal_as<const wchar_t*>(str);
        hr = StringCchCopy(pszBuffer, dwSize, pszStr == NULL ? L"" : pszStr);
        delete context; // 这个将会通过pszStr对内存进行释放。
    }

    return hr;
}

int CSSimpleObjectWrapper::GetStringLength(PWSTR pszString)
{
    return CSSimpleObject::GetStringLength(gcnew String(pszString));
}