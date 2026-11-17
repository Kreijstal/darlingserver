#ifndef _DARLINGSERVER_CONFIG_HPP_
#define _DARLINGSERVER_CONFIG_HPP_

#include <darling-config.h>
#include <string>
#include <utility>

namespace DarlingServer {
	namespace Config {
		// NOTE: you should not rely on these values being `constexpr`;
		//       they may be changed during startup.

		// The directory holding the read-only system image. It is LIBEXEC_PATH unless main() sets another one
		// before any thread is started (see RunMode::resolveLibexecPath for when that is allowed).
		inline std::string& libexecPathStorage() {
			static std::string path = LIBEXEC_PATH;
			return path;
		};

		inline const std::string& libexecPath() {
			return libexecPathStorage();
		};

		inline void setLibexecPath(std::string path) {
			libexecPathStorage() = std::move(path);
		};

		// Where mldr is when it cannot live in the libexec tree (an Android app's native library
		// directory); empty unless main() sets one (see DARLING_MLDR_PATH there).
		inline std::string& mldrPathStorage() {
			static std::string path;
			return path;
		};

		inline void setMldrPath(std::string path) {
			mldrPathStorage() = std::move(path);
		};

		inline std::string defaultMldrPath() {
			if (!mldrPathStorage().empty()) {
				return mldrPathStorage();
			}
			return libexecPath() + "/usr/libexec/darling/mldr";
		};

		// this would actually probably be better as a workqueue construction parameter
		constexpr bool singleThreadedWorkQueue = DSERVER_SINGLE_THREADED;
	};
};

#endif // _DARLINGSERVER_CONFIG_HPP_
