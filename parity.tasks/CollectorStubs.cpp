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

#include "CollectorStubs.h"

#include "MsCompiler.h"
#include "MsLinker.h"
#include "MsDependencyTracker.h"
#include "MsExportGenerator.h"
#include "MsStaticImportGenerator.h"
#include "MsLoaderGenerator.h"
#include "MsSymbolTableGenerator.h"
#include "MsPcrtInitEntryGenerator.h"
#include "MsResourceCompiler.h"
#include "MsResourceConverter.h"

#include <Timing.h>
#include <Log.h>
#include <Exception.h>

namespace parity
{
	namespace tasks
	{

		unsigned int THREADINGAPI TaskStubs::runDependencyTracking(void*)
		{
			utils::Timing::instance().start("Dependency Tracking");

			try {
				parity::tasks::MsDependencyTracker tracker;
				tracker.doWork();
			} catch(const utils::Exception& e) {
				utils::Log::error("while tracking dependencies: %s\n", e.what());
				return 1;
			}

			utils::Timing::instance().stop("Dependency Tracking");

			return 0;
		}

		unsigned int THREADINGAPI TaskStubs::runCompiler(void*)
		{
			utils::Timing::instance().start("Compiler");

			try {
				switch(utils::Context::getContext().getBackendType())
				{
				case utils::ToolchainMicrosoft:
				case utils::ToolchainInterixMS:
					{
						parity::tasks::MsCompiler compiler;
						compiler.doWork();
					}
					break;
				case utils::ToolchainInterixGNU:
				default:
					throw utils::Exception("Invalid backend type requested, or backend not implemented!");
				}
			} catch(const utils::Exception& e) {
				utils::Log::error("while compiling: %s\n", e.what());
				return 1;
			}

			utils::Timing::instance().stop("Compiler");

			return 0;
		}

		unsigned int THREADINGAPI TaskStubs::runLinker(void*)
		{
			utils::Timing::instance().start("Linking");

			try {
				switch(utils::Context::getContext().getBackendType())
				{
				case utils::ToolchainMicrosoft:
				case utils::ToolchainInterixMS:
					{
						parity::tasks::MsLinker linker;
						linker.doWork();
					}
					break;
				case utils::ToolchainInterixGNU:
				default:
					throw utils::Exception("Invalid backend type requested, or backend not implemented!");
				}
			} catch(const utils::Exception& e) {
				utils::Log::error("while linking: %s\n", e.what());
				return 1;
			}

			utils::Timing::instance().stop("Linking");
			return 0;
		}

		unsigned int THREADINGAPI TaskStubs::runMsExportGenerator(void* ptrVec)
		{
			utils::Timing::instance().start("Export Generator");

			try {
				if(!ptrVec)
					throw utils::Exception("pointer to symbols invalid, cannot generate exports!");

				parity::tasks::MsExportGenerator generator(*reinterpret_cast<binary::Symbol::SymbolVector*>(ptrVec));
				generator.doWork();
			} catch(const utils::Exception& e) {
				utils::Log::error("while generating exports: %s\n", e.what());
				return 1;
			}

			utils::Timing::instance().stop("Export Generator");
			return 0;
		}

		unsigned int THREADINGAPI TaskStubs::runMsStaticImportGenerator(void* ptrVec)
		{
			utils::Timing::instance().start("Static Import Generator");

			try {
				if(!ptrVec)
					throw utils::Exception("pointer to symbols invalid, cannot generate static imports!");

				parity::tasks::MsStaticImportGenerator generator(*reinterpret_cast<binary::Symbol::SymbolVector*>(ptrVec));
				generator.doWork();
			} catch(const utils::Exception& e) {
				utils::Log::error("while generating static imports: %s\n", e.what());
				return 1;
			}

			utils::Timing::instance().stop("Static Import Generator");
			return 0;
		}
	
		unsigned int THREADINGAPI TaskStubs::runMsLoaderGenerator(void* ptrMap)
		{
			utils::Timing::instance().start("Loader Generator");

			try {
				if(!ptrMap)
					throw utils::Exception("pointer to imports invalid, cannot generate loader!");

				parity::tasks::MsLoaderGenerator generator(*reinterpret_cast<tasks::BinaryGatherer::ImportHybridityMap*>(ptrMap));
				generator.doWork();
			} catch(const utils::Exception& e) {
				utils::Log::error("while generating loader: %s\n", e.what());
				return 1;
			}

			utils::Timing::instance().stop("Loader Generator");
			return 0;
		}

		unsigned int THREADINGAPI TaskStubs::runMsSymbolTableGenerator(void* ptrMap)
		{
			utils::Timing::instance().start("Symbol Table Generator");

			try {
				if(!ptrMap)
					throw utils::Exception("pointer to symbols invalid, cannot generate symbol table!");

				parity::tasks::MsSymbolTableGenerator generator(*reinterpret_cast<binary::Symbol::SymbolVector*>(ptrMap));
				generator.doWork();
			} catch(const utils::Exception& e) {
				utils::Log::error("while generating symbol table: %s\n", e.what());
				return 1;
			}

			utils::Timing::instance().stop("Symbol Table Generator");
			return 0;
		}

		unsigned int THREADINGAPI TaskStubs::runMsPcrtInitEntryGenerator(void*)
		{
			utils::Timing::instance().start("PcrtInit Entrypoint Generator");

			try {
				parity::tasks::MsPcrtInitEntryGenerator generator;
				generator.doWork();
			} catch(const utils::Exception& e) {
				utils::Log::error("while generating PcrtInit entrypoint: %s\n", e.what());
				return 1;
			}

			utils::Timing::instance().stop("PcrtInit Entrypoint Generator");
			return 0;
		}

		unsigned int THREADINGAPI TaskStubs::runMsResourceCompiler(void* rcargs)
		{
			utils::Timing::instance().start("Resource Compiler");

			try {
				if(!rcargs)
					throw utils::Exception("missing arguments for MsResourceCompiler!");

				switch(utils::Context::getContext().getBackendType()) {
				case utils::ToolchainMicrosoft:
				case utils::ToolchainInterixMS:
					{
						parity::tasks::MsResourceCompiler compiler(*reinterpret_cast<MsResourceCompiler::Constructible*>(rcargs));
						compiler.doWork();
					}
					break;
				case utils::ToolchainInterixGNU:
				default:
					throw utils::Exception("Invalid backend type requested, or backend not implemented!");
				}
			} catch(const utils::Exception& e) {
				utils::Log::error("while resource compiling: %s\n", e.what());
				return 1;
			}

			utils::Timing::instance().stop("Resource Compiler");

			return 0;
		}

		unsigned int THREADINGAPI TaskStubs::runMsResourceConverter(void* rcargs)
		{
			utils::Timing::instance().start("Resource Converter");

			try {
				if(!rcargs)
					throw utils::Exception("missing arguments for MsResourceConverter!");

				switch(utils::Context::getContext().getBackendType()) {
				case utils::ToolchainMicrosoft:
				case utils::ToolchainInterixMS:
					{
						parity::tasks::MsResourceConverter compiler(*reinterpret_cast<MsResourceConverter::Constructible*>(rcargs));
						compiler.doWork();
					}
					break;
				case utils::ToolchainInterixGNU:
				default:
					throw utils::Exception("Invalid backend type requested, or backend not implemented!");
				}
			} catch(const utils::Exception& e) {
				utils::Log::error("while resource compiling: %s\n", e.what());
				return 1;
			}

			utils::Timing::instance().stop("Resource Converter");

			return 0;
		}
	}
}

