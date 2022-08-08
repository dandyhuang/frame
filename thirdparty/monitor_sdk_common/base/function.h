//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 13:27
//  @file:      function.h
//  @author:    
//  @brief:     generic function object and bind
//
//********************************************************************

#ifndef COMMON_BASE_FUNCTION_H
#define COMMON_BASE_FUNCTION_H
#pragma once

// �������ü�����ͨ�ú�������Ͱ󶨺���
//
// Function ��ģ����һ��ͨ�õĺ�������ʵ�֡��� Bind ����һ��ʹ�ã��ṩ�����Ͱ�
// ȫ�ķ�ʽ���в����Ŀ��ﻯ���Լ������հ���
//
//
// �ڴ����ʹ���
//
// Function ������Ӧ���� const ���õķ�ʽ���ݣ����Կ����ķ�ʽ�洢�������ڲ�
// ͨ�����ü�������洢״̬����˲���Ҫ��ʽ��ɾ����
//
// ͨ�� const ���ô��ݵ�ԭ����Ϊ�˱��ⲻ��Ҫ�����ü������������Ķ��⿪����
//
// ʹ��ʾ��:
//
// // ��򵥵����ӣ���һ��һ��ĺ�����
// int Return5() { return 5; }
// Function<int(int)> func_cb = Bind(&Return5);
// cout << func_cb(5);  // ��� 5.
//
// void PrintHi() { cout << "hi."; }
// Function<void ()> void_func_cb = Bind(&PrintHi);
// cout << void_func_cb();  // ���: hi.
//
//
// // ���õĳ��ϣ����෽���������ص�ʱ�����á�
// class SomeClass {
//  public:
//   int Foo() { return 4; }
//   void PrintWhy() { cout << "why???"; }
// };
// SomeClass object;
// Function<int(void)> cb =
//     Bind(&SomeClass::Foo, &object);
// cout << cb();  // ��� 4.
//
// Function<void ()> void_cb = Bind(&SomeClass::PrintWhy, &object);
// void_cb();  // ��� why???
//
// // ע��: ����ȷ�������������󶨵Ķ���������ڡ���������ʱ�ͻ�����⣬
// //       ��������õĽ�֡�
// // ��ʱ���������֧�����ü�����һ���취��
//
// // ��֧�����ü�������ķ���.
// // ����Ҫ�� RefCountedBase ģ����������������Լ���������Ϊģ�������
//
// #include "commob/base/red_counted.hpp"
//
// class Ref : public RefCountedBase<Ref>
// {
// public:
//     int Foo() { return 3; }
//     void PrintBye() { cout << "bye."; }
// };
//
// scoped_refptr<Ref> ref = new Ref();
// Function<int(void)> ref_cb = Bind(&Ref::Foo, ref.get());
// cout << ref_cb();  // ��� 3.
//
// Function<void ()> void_ref_cb = Bind(&Ref::PrintBye, ref.get());
// void_ref_cb();  // ���: bye.
// // ÿ�� Function ���󶼳��ж԰󶨵Ķ����һ�����ã�������Ҳ����Ӱ�졣
//
//
// // �� Function �����ٴ� Bind��
// int Add(int x, int y)
// {
//     return x + y;
// }
//
// Function<int (int x, int y)> add = Bind(Add);
// cout << add(1, 2); // ��� 3
//
// // �ٴΰ�
// Function<int (int x)> inc = Bind(add, 1);
// cout << inc(1);    // ��� 2
//
// // ��һ����
// Function<int ()> two = Bind(inc, 1);
// cout << two();     // ��� 2
//
//
// // �ı䴫����ʱ����Ϊ��
// // Ĭ������£�������ֵ�ķ�ʽ�󶨣�����Ҳ����ͨ�� ConstRef ���κ�����ǿ����
// // ���÷�ʽ�󶨲�����
// int Identity(int n) { return n; }
// int value = 1;
// Function<int(void)> bound_copy_cb = Bind(&Identity, value);
// Function<int(void)> bound_ref_cb = Bind(&Identity, ConstRef(value));
// cout << bound_copy_cb();  // ��� 1.
// cout << bound_ref_cb();   // ��� 1.
// value = 2;                // ������� value ��Ϊ 2
// cout << bound_copy_cb();  // ��� 1.
// cout << bound_ref_cb();   // ��� 2 �ˣ�˵�� bound_ref_cb �����ð��˲���.
//
// Unretained() ��������ڰ󶨵�֧�����ü����Ķ�����ʱ����ȥ�Ӽ����ü�����ȷ��
// ������ʱû����ʱ��������΢���һЩ���ܡ�
//
// �����������󲿷ָ� tr1::function �÷�һ����
//
// Function<void ()> f, g;
// g = f;              // ��ֵ
// f = NULL;           // ��ֵΪ��
// if (f == g) { ... } // �ж��Ƿ�ָ��ͬһ��ʵ��
// if (f) { ... }      // �жϷǿ�
// if (!f) { ... }     // �ж�Ϊ��
//
// ʵ�֣�
//
// �����������:
//   1) Function ��ģ��.
//   2) Bind() ����.
//   3) ������װ (���� Unretained() �� ConstRef()).
//
// Function �����һ��ͨ�õĺ���ָ�롣���ڲ����洢�˴����û������ڲ�״̬������
// Ŀ�꺯�������а��˵Ĳ�����
//
// Bind �����򴴽�������״̬���������� Function ����
//
// Bind() Ĭ�ϻ�洢���в�������Ŀ��������һ᳢�����ӱ��󶨵Ķ�������ü�����
//
// Ϊ������ı���Щ��Ϊ��������һЩ���������κ��������� Unretained() ��
// ConstRef())��
//
// ConstRef() ������ tr1::cref.  Unretained() �� Function ���е�.
//
//
// �� C++ ��׼��� tr1::function/bind �Ĳ���:
//
// tr1::function �� tr1::bind ��ֵ�󶨲�����Ҳ��ֵ����������ǲ������ü�����
// �����󶨵Ĳ����Ƚϡ��ء���ʱ�򣬱���һ������ string ���� map<>��function ����
// ÿ������һ�Σ���Щ������Ҳ���Ÿ���һ�Ρ�
//
// ����ǽ����� C++0x��Ŀǰ������ 0x0B����֧����ֵ���ã����Լ��ٿ����������վ�
// ���ܱ��⣬���ٴ��ݽ���������������ʱ��
//
// ���� tr1::function ��ʵ��Ҳ��һ���ŵ㣬���ǵ��󶨵Ĳ�������ʱ���ܴ�С������
// �ض��Ĵ�Сʱ����Ϣֱ�ӷ��� function �����ڣ�����Ҫ���䶯̬�ڴ档���������
// tcmalloc�����䶯̬�ڴ�Ŀ���Ҳ�������⡣
//
// �� valgrind �� GCC �ϲ�����ʾ������һ�� tr1::bind�����ÿ�������������Ρ���
// ��ֵ���� function ����ʱ��ÿ�������ֶ���������һ�Ρ������ʹ�� Function����
// ʹ��ֵ���ݣ����� const ���ô��ݣ�Ҳֻ�Ƕ���һЩ����Ҫ�����ü���ά���Ŀ���
// ���ѡ�
//
// ���⣬�� Function �󶨵��෽��ʱ�������������֧�����ü������ͻ��Զ�����
// �������ü��������ڼ���Դ�����Ǻ�������ġ�tr1::function û�����Ƶ����ԡ�
//
//
// �� Closure �Ĳ��죺
//
// �����Ѿ�����һ�� Closure�������ṩ������İ󶨵Ĺ��ܡ����� Closure ��֧����
// �ü�����ֻ֧�ֵ��ú��Զ�ɾ�����˹�ɾ�����Զ�ɾ����ģʽ����Ҫ׷���Ƿ��Ѿ���
// �������˹�ɾ��������Ҫ��ʽ delete�����ߴ��� scoped_ptr������Щ�ӿ���Ҫ�Զ�
// ɾ���� closure����Щ���෴��������������ͣ������ڼ�Ҳ�޷����֣�ֻ�ܿ�����
// �ڼ�顣���� NewClosure �Բ�������ƥ���Ҫ����ȫƥ�䣬��������ı��������
// Ϣ���ѿ�����
//
// Closure �Ĳ�����֧���βΣ��� Function ֧�֣���������ߴ���Ŀɶ��ԣ�
// Closure<void, int, int>* done;
// const Closure<void (int error_code, int tranfered_length)>& done;
//
// ���⣬Closure ��֧�ֶ��ٴΰ󶨡�Ҳ������Ϊ�������󴫸� STL �㷨��Ҳ��֧�ְ�
// �� const ���á�

#include "thirdparty/monitor_sdk_common/base/function/function.h"
#include "thirdparty/monitor_sdk_common/base/function/bind.h"

#endif // COMMON_BASE_FUNCTION_H
