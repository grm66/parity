/****************************************************************\
*                                                                *
* Copyright (C) 2007 by Markus Duft <markus.duft@salomon.at>     *
*                                                                *
* This file is part of parity.                                   *
*                                                                *
* parity is free software: you can redistribute it and/or modify *
* it under the terms of the GNU Lesser General Public License as *
* published by the Free Software Foundation, either version 3 of *
* the License, or (at your option) any later version.            *
*                                                                *
* parity is distributed in the hope that it will be useful,      *
* but WITHOUT ANY WARRANTY; without even the implied warranty of *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  *
* GNU Lesser General Public License for more details.            *
*                                                                *
* You should have received a copy of the GNU Lesser General      *
* Public License along with parity. If not,                      *
* see <http://www.gnu.org/licenses/>.                            *
*                                                                *
\****************************************************************/

#ifndef __LOADERINIT_H__
#define __LOADERINIT_H__

//
// WARNING: when changing this, update Inspector.h from
// parity.inspector too!
//

#if defined(_WIN32) || (defined(__INTERIX) && (!defined(__GNUC__) || __GNUC__ == 3 ))
#  pragma pack(push, 2)
#  define STRUCT_PACKING_ATTRIBUTE
#elif defined(__GNUC__)
#  define STRUCT_PACKING_ATTRIBUTE __attribute__((packed, aligned(2)))
#else
#  error "don't know how to handle structure alignment!"
#endif

typedef struct STRUCT_PACKING_ATTRIBUTE {
	const char*		name;
	void*			import;
	unsigned short	ordinal;
	const char*		library;
} ImportItem;

typedef struct STRUCT_PACKING_ATTRIBUTE {
	const char*		name;
	ImportItem*		imports;
} LibraryItem;

typedef struct STRUCT_PACKING_ATTRIBUTE {
	LibraryItem*	libraries;
} LoaderTable;

#if defined(_WIN32) || (defined(__INTERIX) && (!defined(__GNUC__) || __GNUC__ == 3 ))
#  pragma pack(pop)
#endif

#undef STRUCT_PACKING_ATTRIBUTE

typedef enum {
	StatusFailure = 0,
	StatusSuccess = 1
} LoaderStatus;

//
// WARNING: this is the same as in utils::ContextGen, keep in sync!
//
typedef enum {
	SubsystemUnknown				= 0,
	SubsystemNative					= 1,
	SubsystemWindowsGui				= 2,
	SubsystemWindowsCui				= 3,
	SubsystemPosixCui				= 7,
	SubsystemWindowsCeGui			= 9,
	SubsystemEFIApplication			= 10,
	SubsystemEFIBootServiceDriver	= 11,
	SubsystemEFIRuntimeDriver		= 12,
	SubsystemEFIROM					= 13,
	SubsystemXbox					= 14
} LoaderSubsystemType;

extern LoaderStatus ParityLoaderInit();

extern LoaderTable*			ParityLoaderGetGeneratedTable(void);
extern const char*			ParityLoaderGetGeneratedRunPath(void);
extern LoaderSubsystemType	ParityLoaderGetGeneratedSubsystem(void);
extern const char*			ParityLoaderGetGeneratedImageName(void);

#endif

