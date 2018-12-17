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

#include "Threading.h"
#include "Exception.h"
#include "Log.h"
#include "Timing.h"
#include "Statistics.h"

#include <cerrno>
#include <cstdlib>

#ifdef _WIN32
#  include <process.h>
#  include <streambuf>
#else
#  include <unistd.h>
#  include <pthread.h>
#  include <sys/wait.h>
#endif

namespace parity
{
	namespace utils
	{
		void Threading::waitForOneThread(HandleMap::iterator it)
		{
			#ifdef _WIN32
				DWORD exitStatus;
				Log::verbose("waiting for thread %ld...\n", it->first);
				WaitForSingleObject(it->second, INFINITE);
				GetExitCodeThread(it->second, &exitStatus);
				CloseHandle(it->second);

				if(exitStatus != 0)
				{
					throw Exception("Thread %ld exited abnormally with code: %d!", it->first, exitStatus);
				}
			#elif POSIX_THREADING_MODEL == POSIX_THREADS
				void* val;
				Log::verbose("joining posix thread %ld...\n", it->first);

				if(pthread_join(it->first, &val) != 0) {
					throw Exception("cannot join posix thread %d!", it->first);
				}

				if((unsigned int)val != 0)
				{
					throw Exception("Posix thread %ld exited abnormally with code: %d!", it->first, val);
				}
			#elif POSIX_THREADING_MODEL == POSIX_FORK
				int ret = 0;
				Log::verbose("waiting for forked child %ld.\n", it->first);

				if(waitpid(it->first, &ret, WUNTRACED) != it->first)
				{
					throw Exception("cannot wait for forked child %ld: %s.", it->first, ::strerror(errno));
				}

				if(WIFEXITED(ret) && WEXITSTATUS(ret) != 0)
				{
					throw Exception("forked child %ld exited abnormally with code %d.", it->first, WEXITSTATUS(ret));
				} else if(!WIFEXITED(ret)) {
					throw Exception("forked child %ld did not terminate normally.\n", it->first);
				}
			#endif
		}

		void Threading::synchronize()
		{
			for(HandleMap::iterator it = handles_.begin(); it != handles_.end(); ++it)
			{
				waitForOneThread(it);
			}

			handles_.clear();
		}

		/* not needed right now. commented out for future
		 * reference. maybe this will be needed someday, when
		 * there are tasks that require a specific order, but
		 * still some of them can run in parallel...
		 *
		void Threading::synchronize(long id)
		{
			HandleMap::iterator pos = handles_.find(id);
			if(pos != handles_.end())
			{
				waitForOneThread(pos);
			}
		}
		 */

		#ifdef _WIN32
		# define THR_MAYBE_UNUSED(x)
		#else
		# define THR_MAYBE_UNUSED(x) x
		#endif

		long Threading::run(ThreadingFunction method, void* data, bool THR_MAYBE_UNUSED(allowDataSeparation))
		{
			long threadID;

			#ifdef _WIN32
				HANDLE handle = (HANDLE)_beginthreadex(0, 0, method, data, 0, (unsigned*)&threadID);

				if(!handle)
					throw Exception("cannot create thread: %s", ::strerror(errno));

				Log::verbose("created new thread with ID %ld.\n", threadID);

				handles_[threadID] = (void*)handle;
			#elif POSIX_THREADING_MODEL == POSIX_THREADS
				if(pthread_create((pthread_t*)&threadID, 0, (void*(*)(void*))method, data) != 0)
					throw Exception("cannot create thread: %s", ::strerror(errno));

				Log::verbose("created posix thread with ID %ld.\n", threadID);

				handles_[threadID] = 0;
			#elif POSIX_THREADING_MODEL == POSIX_FORK
				if(allowDataSeparation)
				{
					threadID = fork();

					switch(threadID) {
					case -1:
						throw Exception("cannot fork child: %s", ::strerror(errno));
					case 0:
						Timing::instance().forked();
						Statistics::instance().forked();
						exit(method(data));
					default:
						Log::verbose("created forked child with id %ld.\n", threadID);
						handles_[threadID] = 0;
						break;
					}
				} else {
					Log::verbose("preventing data separation by running single threaded job!\n");
					method(data);
					return 1;
				}
			#elif POSIX_THREADING_MODEL == POSIX_NONE
				Log::verbose("running single threaded job, exit code will be lost!\n");
				method(data);
				return 1;
			#else
				throw Exception("don't know how to thread!");
			#endif
			return threadID;
		}
	}
}

