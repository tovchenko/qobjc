/*
    This file is part of the q-objc runtime library.
    Copyright (C) 2008-2009 Taras Tovchenko Victorovich <doctorset@gmail.com>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// tests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "miscTest.h"
#include "classTest.h"
#include "encodeTest.h"
#include "hashTest.h"
#include "msgsendTest.h"
#include "declTest.h"

#include <crtdbg.h>

Q_ERROR_MESSAGE(er0, "Detected memory leaks!\n");

int
_tmain(int argc, _TCHAR* argv[]) {
    AutoreleasePool pool = q_allocAutoreleasePool();

    // _CrtSetBreakAlloc(83775);

    q_initializeEnvironment();

    runMiscTest();
    runClassTest();
    runEncodeTest();
    runHashTest();
    runMsgSendTest();
    runDeclTest();

    q_finalizeEnvironment();
    Q_RELEASE(pool);

    if (_CrtDumpMemoryLeaks()) q_throwError(er0);
	return 0;
}

