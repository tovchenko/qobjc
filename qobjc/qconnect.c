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

#include "stdafx.h"
#include "qconnect.h"
#include "qselector.h"
#include "qclass.h"
#include "qobject.h"
#include "qprotocol.h"
#include "qloading.h"


// Need load 'Object' and 'Protocol' classes first and unload last
static void        _loadSystemClasses();
static void        _unloadSystemClasses();


void
q_initializeEnvironment() {
    q_initializeSelectorModule();

    _loadSystemClasses();
}

void
q_finalizeEnvironment() {
    q_finalizeClassModule();

    _unloadSystemClasses();

    q_finalizeSelectorModule();
    q_finalizeLoaderModule();
    q_finalizeMemoryModule();
}

static void
_loadSystemClasses() {
    q_loadRootClass();
    q_loadProtocolClass();
}

static void
_unloadSystemClasses() {
    q_unloadProtocolClass();
    q_unloadRootClass();
}